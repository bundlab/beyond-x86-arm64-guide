# Chapter 2: Microarchitecture & Execution Models: Registers, Memory Ordering, and SIMD

When porting high-performance applications, multi-threaded frameworks, or systems-level code from x86_64 to ARM64, syntax compatibility is only half the battle. While a C++, Go, or Rust codebase may compile cleanly under both architectures, low-level microarchitectural divergences can introduce silent data corruption, subtle concurrency deadlocks, and severe performance regressions.

This chapter dissects the core microarchitectural differences between CISC (x86_64) and RISC (ARM64) processors across three critical domains: register files and calling conventions, memory consistency models, and vectorization primitives.

---

## 2.1 Register Layouts & Calling Conventions: x86-64 vs. ARM64

At the assembly and compiler backend level, the organization of general-purpose registers dictates how efficiently functions execute, how many arguments can be passed without stack overhead, and how register allocation is handled.

### x86-64 Register File (The Legacy Legacy)
The x86-64 register set is burdened by decades of backward compatibility stretching back to the 16-bit 8086 and 32-bit IA-32 architectures:
* **General Purpose Registers (GPRs)**: Sixteen 64-bit registers (`rax`, `rbx`, `rcx`, `rdx`, `rsi`, `rdi`, `rbp`, `rsp`, and `r8` through `r15`).
* **Irregular Roles**: While some general-purpose registers can be used interchangeably, many carry implicit historical baggage. For instance, multiplication and division instructions (`mul`, `div`) hardcode dependencies on `rax`, `rdx`, and `rcx`.
* **Calling Conventions**: System V AMD64 ABI (Linux/macOS) passes the first six integer/pointer arguments in registers (`rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9`), while Microsoft x64 (Windows) uses `rcx`, `rdx`, `r8`, `r9`.

### ARM64 Register File (Clean-Sheet Design)
ARM64 was designed from scratch as a clean 64-bit architecture without legacy 16-bit or 32-bit constraints:
* **General Purpose Registers**: Thirty-one 64-bit registers, denoted as `x0` through `x30` (or `w0` through `w30` when accessing the lower 32 bits).
* **The Zero Register (`xzr` / `wzr`)**: Reading from `xzr` always returns zero; writing to it discards the data. This eliminates the need for dedicated hardware constants or zero-clearing instructions.
* **The Link Register (`x30` / `lr`)**: Unlike x86, which pushes return addresses onto the stack during a `CALL` instruction, ARM64 stores the return address directly in register `x30`, accelerating function prologue/epilogue execution.
* **Calling Convention**: The ARM64 procedure call standard uses registers `x0` through `x7` for passing integer/pointer arguments and returning function values, drastically reducing memory stack pressure for function-heavy code.

---

## 2.2 Memory Ordering Models: TSO vs. Weak Memory Ordering

The single most dangerous trap when moving concurrent, lock-free, or low-level systems code from x86 to ARM64 is the underlying memory consistency model.


```

```
   x86-64 (TSO)                              ARM64 (Weak Ordering)

```

┌─────────────────────────────────┐       ┌─────────────────────────────────┐
│ Store cannot pass older Load    │       │ Free reordering of Loads/Stores │
│ Strongly ordered by hardware    │       │ Requires explicit barrier (DMB) │
└─────────────────────────────────┘       └─────────────────────────────────┘

```

### Total Store Order (TSO) on x86-64
Intel and AMD processors implement **Total Store Order (TSO)**. Under TSO:
* A store (write) operation cannot be reordered by the CPU hardware with older load (read) operations from different memory locations.
* Hardware store buffers may forward pending writes to subsequent reads by the *same* core, but other cores observe writes in a globally consistent order.
* **The Developer Trap**: Because x86 hardware implicitly enforces strict ordering guarantees, developers frequently write lock-free data structures, singleton initializations, or flag checks with relaxed or missing memory barriers, and the code runs flawlessly on Intel/AMD hardware.

### Weak Memory Ordering on ARM64
ARM64 implements a **Weakly Ordered** memory architecture:
* To maximize instruction-level parallelism, power efficiency, and out-of-order execution throughput, both the CPU hardware and the compiler are free to aggressively reorder loads and stores across arbitrary memory addresses.
* If Thread A updates data and then sets a ready flag, an ARM64 core running Thread B may observe the flag change *before* it observes the underlying data updates in cache.
* **Mitigation**: Developers must use explicit acquire-release semantics (e.g., C++ `std::memory_order_acquire` / `std::memory_order_release`) or hardware memory barrier instructions (`DMB` - Data Memory Barrier, `DSB` - Data Synchronization Barrier) to enforce sequencing when sharing state across threads without heavy mutexes.

---

## 2.3 SIMD & Vectorization: SSE/AVX vs. NEON & SVE

Data-level parallelism is vital for image processing, cryptography, database scans, and machine learning inference. 

### x86 Vector Extensions (SSE, AVX2, AVX-512)
* x86 relies on widening vector registers across generations: 128-bit (`xmm`), 256-bit (`ymm` via AVX2), and 512-bit (`zmm` via AVX-512).
* **Thermal Throttling**: Executing wide AVX-512 instructions on many x86 server chips forces the CPU to draw massive surge currents, triggering automatic clock frequency down-throttling to maintain thermal design power (TDP) limits.

### ARM NEON (Advanced SIMD)
* ARM64 incorporates **NEON** as a standard architectural feature, providing thirty-two 128-bit vector registers (`v0` through `v31`) accessible as byte, half-word, word, or double-word lanes.
* NEON executes vector arithmetic with exceptional energy efficiency, making it the primary engine behind real-time mobile media codecs and edge AI inference.

### ARM Scalable Vector Extension (SVE & SVE2)
* Unlike fixed-width vector extensions (AVX or NEON), **SVE** is *vector-length agnostic*. 
* SVE code is compiled without assuming a specific register width. Whether executed on a small edge core with 128-bit vectors or a high-end supercomputing/server core with 2048-bit vectors, the exact same binary executes correctly and scales performance dynamically without recompilation.

---
