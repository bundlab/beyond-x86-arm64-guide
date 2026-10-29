#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if defined(__aarch64__) || defined(_M_ARM64)
#include <arm_neon.h>
#define HAS_NEON 1
#else
#define HAS_NEON 0
#endif

#define ARRAY_SIZE 10000000

// Scalar baseline implementation
void multiply_scalar(const float* a, const float* b, float* out, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        out[i] = a[i] * b[i];
    }
}

// ARM64 NEON vectorized implementation (processes 4 floats per cycle)
void multiply_neon(const float* a, const float* b, float* out, size_t n) {
#if HAS_NEON
    size_t i = 0;
    // Process 4 elements per iteration using 128-bit NEON registers
    for (; i + 3 < n; i += 4) {
        float32x4_t va = vld1q_f32(&a[i]);
        float32x4_t vb = vld1q_f32(&b[i]);
        float32x4_t vres = vmulq_f32(va, vb);
        vst1q_f32(&out[i], vres);
    }
    // Handle remaining elements scalar-style
    for (; i < n; ++i) {
        out[i] = a[i] * b[i];
    }
#else
    // Fallback if compiled on non-ARM architecture without emulation
    multiply_scalar(a, b, out, n);
#endif
}

int main(void) {
    printf("==================================================\n");
    printf("   LAB 03: ARM64 NEON SIMD Vectorization Benchmark\n");
    printf("==================================================\n");

#if HAS_NEON
    printf("[+] Hardware ARM64 NEON Support: DETECTED\n");
#else
    printf("[!] Warning: Running on non-ARM64 host (NEON code will fallback or require emulation).\n");
#endif

    float* a = (float*)malloc(ARRAY_SIZE * sizeof(float));
    float* b = (float*)malloc(ARRAY_SIZE * sizeof(float));
    float* out_scalar = (float*)malloc(ARRAY_SIZE * sizeof(float));
    float* out_neon = (float*)malloc(ARRAY_SIZE * sizeof(float));

    // Initialize arrays with sample data
    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
        a[i] = (float)(i % 100) + 1.5f;
        b[i] = (float)(i % 50) + 2.0f;
    }

    // Benchmark Scalar
    clock_t start = clock();
    multiply_scalar(a, b, out_scalar, ARRAY_SIZE);
    clock_t end = clock();
    double scalar_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("[*] Scalar Multiplication Time: %.4f seconds\n", scalar_time);

    // Benchmark NEON
    start = clock();
    multiply_neon(a, b, out_neon, ARRAY_SIZE);
    end = clock();
    double neon_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("[*] NEON Vectorized Time:      %.4f seconds\n", neon_time);

    if (scalar_time > 0) {
        printf("[+] Speedup Factor:            %.2fx\n", scalar_time / neon_time);
    }

    // Verify correctness of results
    int success = 1;
    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
        if (out_scalar[i] != out_neon[i]) {
            success = 0;
            break;
        }
    }

    if (success) {
        printf("[✔] Verification PASSED: Scalar and NEON outputs match perfectly!\n");
    } else {
        printf("[✘] Verification FAILED: Mismatch detected between scalar and NEON results.\n");
    }

    free(a);
    free(b);
    free(out_scalar);
    free(out_neon);

    return success ? 0 : 1;
}