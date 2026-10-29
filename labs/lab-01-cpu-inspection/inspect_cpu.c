#include <stdio.h>
#include <runtime/config.h>

void print_architecture(void) {
#if defined(__x86_64__) || defined(_M_X64)
    printf("Detected Architecture: x86_64 / AMD64\n");
    printf("Execution Model: CISC (Complex Instruction Set Computer)\n");
    printf("Memory Model: Total Store Order (TSO - Strongly Ordered)\n");
#elif defined(__aarch64__) || defined(_M_ARM64)
    printf("Detected Architecture: ARM64 / AArch64\n");
    printf("Execution Model: RISC (Reduced Instruction Set Computer)\n");
    printf("Memory Model: Weak Memory Ordering (Relaxed)\n");
#else
    printf("Detected Architecture: Unknown / Unsupported\n");
#endif
}

int main(void) {
    printf("==================================================\n");
    printf("   LAB 01: CPU ARCHITECTURE & HARDWARE INSPECTION  \n");
    printf("==================================================\n");
    print_architecture();
    printf("Pointer Size: %lu bytes (%lu bits)\n", sizeof(void*), sizeof(void*) * 8);
    return 0;
}
