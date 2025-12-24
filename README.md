# Low-Latency Queue in C++ (Mutex vs Lock-Free)

This project explores the performance differences between **lock-based** and **lock-free** queue designs in C++ under multithreaded workloads.  
The goal is to understand how synchronization choices impact **latency** and **throughput** in concurrent systems.

---

## 🚀 Project Overview

The project implements and benchmarks two queue designs:

1. **Mutex-Based Queue**
   - Uses `std::mutex` to ensure thread safety
   - Simple and correct baseline
   - Suffers from lock contention under concurrency

2. **Lock-Free SPSC Queue**
   - Single-Producer Single-Consumer (SPSC) design
   - Uses `std::atomic` for synchronization
   - Avoids locks and context switching
   - Optimized for low-latency paths

Both implementations are benchmarked using microsecond-resolution timing.

---

## 🛠️ Tech Stack

- **Language:** C++17
- **Concurrency:** `std::thread`, `std::mutex`, `std::atomic`
- **Data Structures:** STL containers, ring buffer
- **Benchmarking:** `std::chrono`
- **Toolchain:** GCC / MinGW / MSYS2 / Linux

---

## 🧪 Benchmark Design

- One producer thread
- One consumer thread
- One million enqueue/dequeue operations
- Total execution time measured in microseconds

---

## 📊 Sample Output

