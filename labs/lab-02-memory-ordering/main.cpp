#include <iostream>
#include <thread>
#include <atomic>
#include <vector>

// Shared atomic flags for testing memory reordering
std::atomic<int> flag_a{0};
std::atomic<int> flag_b{0};
std::atomic<int> reorder_detected_count{0};

void worker_thread_1(int iterations, std::memory_order order) {
    for (int i = 0; i < iterations; ++i) {
        flag_a.store(1, order);
        flag_b.store(1, order);
        
        // Reset flags for the next iteration
        flag_a.store(0, std::memory_order_relaxed);
        flag_b.store(0, std::memory_order_relaxed);
    }
}

void worker_thread_2(int iterations, std::memory_order order) {
    for (int i = 0; i < iterations; ++i) {
        // Under weak memory ordering with relaxed semantics, 
        // the CPU or compiler may observe flag_b set before flag_a.
        int b_val = flag_b.load(order);
        int a_val = flag_a.load(order);

        if (b_val == 1 && a_val == 0) {
            reorder_detected_count.fetch_add(1, std::memory_order_relaxed);
        }
    }
}

void run_test(const std::string& mode_name, std::memory_order order) {
    flag_a.store(0);
    flag_b.store(0);
    reorder_detected_count.store(0);

    int iterations = 1000000;
    
    // Wrapped in lambdas to ensure clean argument decay and avoid GCC 13 template forwarding errors
    std::thread t1([=]() { worker_thread_1(iterations, order); });
    std::thread t2([=]() { worker_thread_2(iterations, order); });

    t1.join();
    t2.join();

    std::cout << "[*] Mode: " << mode_name 
              << " | Reorder events detected: " << reorder_detected_count.load() 
              << " out of " << iterations << " iterations\n";
}

int main() {
    std::cout << "====================================================\n";
    std::cout << "   LAB 02: Memory Ordering (TSO vs. Weak Model)    \n";
    std::cout << "====================================================\n";

    // Test 1: Relaxed memory ordering allows hardware/compiler reordering
    run_test("Relaxed Ordering (std::memory_order_relaxed)", std::memory_order_relaxed);

    // Test 2: Sequentially consistent ordering enforces strict global order
    run_test("Sequential Consistency (std::memory_order_seq_cst)", std::memory_order_seq_cst);

    std::cout << "[✔] Memory ordering test execution finished successfully.\n";
    return 0;
}