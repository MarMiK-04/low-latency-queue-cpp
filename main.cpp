#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <atomic>
#include <vector>
#include <chrono>

// =======================================================
// Mutex-based Queue (Baseline)
// =======================================================
class MutexQueue {
private:
    std::queue<int> q;
    std::mutex m;

public:
    void push(int value) {
        std::lock_guard<std::mutex> lock(m);
        q.push(value);
    }

    bool pop(int &value) {
        std::lock_guard<std::mutex> lock(m);
        if (q.empty()) return false;
        value = q.front();
        q.pop();
        return true;
    }
};

// =======================================================
// Lock-Free Single Producer Single Consumer Queue
// =======================================================
class LockFreeQueue {
private:
    static const size_t SIZE = 1024 * 1024;
    std::vector<int> buffer;
    std::atomic<size_t> head;
    std::atomic<size_t> tail;

public:
    LockFreeQueue() : buffer(SIZE), head(0), tail(0) {}

    bool push(int value) {
        size_t t = tail.load(std::memory_order_relaxed);
        size_t next = (t + 1) % SIZE;

        if (next == head.load(std::memory_order_acquire)) {
            return false; // queue full
        }

        buffer[t] = value;
        tail.store(next, std::memory_order_release);
        return true;
    }

    bool pop(int &value) {
        size_t h = head.load(std::memory_order_relaxed);

        if (h == tail.load(std::memory_order_acquire)) {
            return false; // queue empty
        }

        value = buffer[h];
        head.store((h + 1) % SIZE, std::memory_order_release);
        return true;
    }
};

// =======================================================
// Producer / Consumer
// =======================================================
void producer_mutex(MutexQueue &q, int count) {
    for (int i = 0; i < count; i++) {
        q.push(i);
    }
}

void consumer_mutex(MutexQueue &q, int count) {
    int value;
    int consumed = 0;
    while (consumed < count) {
        if (q.pop(value)) {
            consumed++;
        }
    }
}

void producer_lockfree(LockFreeQueue &q, int count) {
    for (int i = 0; i < count; i++) {
        while (!q.push(i)) {
            // spin until space is available
        }
    }
}

void consumer_lockfree(LockFreeQueue &q, int count) {
    int value;
    int consumed = 0;
    while (consumed < count) {
        if (q.pop(value)) {
            consumed++;
        }
    }
}

// =======================================================
// Benchmark Runner
// =======================================================
template<typename Func>
long long benchmark(Func f) {
    auto start = std::chrono::high_resolution_clock::now();
    f();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}

int main() {
    const int N = 1'000'000;

    // ---------------- Mutex Queue ----------------
    {
        MutexQueue mq;
        long long time = benchmark([&]() {
            std::thread t1(producer_mutex, std::ref(mq), N);
            std::thread t2(consumer_mutex, std::ref(mq), N);
            t1.join();
            t2.join();
        });

        std::cout << "Mutex Queue Time: " << time << " us\n";
    }

    // ---------------- Lock-Free Queue ----------------
    {
        LockFreeQueue lfq;
        long long time = benchmark([&]() {
            std::thread t1(producer_lockfree, std::ref(lfq), N);
            std::thread t2(consumer_lockfree, std::ref(lfq), N);
            t1.join();
            t2.join();
        });

        std::cout << "Lock-Free Queue Time: " << time << " us\n";
    }

    return 0;
}


