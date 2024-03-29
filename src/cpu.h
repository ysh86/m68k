#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define SIZE_OF_VECTORS 0x0400

typedef void * context_t;
typedef uint8_t *(*mmu_v2r_t)(context_t ctx, uint32_t vaddr);
typedef uint32_t (*mmu_r2v_t)(context_t ctx, uint8_t *raddr);
typedef void (*syscall_t)(context_t ctx);

struct cpu_tag {
    // machine
    context_t ctx;
    mmu_v2r_t mmuV2R;
    mmu_r2v_t mmuR2V;
    syscall_t syscallHook;

    // core: fetch, decode, exec
    // the only one core in Musashi lib.
};
#ifndef _CPU_T_
#define _CPU_T_
typedef struct cpu_tag cpu_t;
#endif

void init(
    cpu_t *pcpu,
    context_t ctx,
    mmu_v2r_t v2r,
    mmu_r2v_t r2v,
    syscall_t syscallHook,
    uint32_t sp, uint32_t pc);

uint32_t getISP(cpu_t *pcpu);
uint32_t getSP(cpu_t *pcpu);
uint32_t getPC(cpu_t *pcpu);
uint32_t getD0(cpu_t *pcpu);
void setD0(cpu_t *pcpu, uint32_t d);
uint32_t getD1(cpu_t *pcpu);
uint32_t getA0(cpu_t *pcpu);
uint32_t rte(cpu_t *pcpu, uint16_t ustatus, uint32_t usp, uint32_t isp, uint32_t ppc, uint32_t pc);

uint16_t fetch(cpu_t *pcpu);
void decode(cpu_t *pcpu);
void exec(cpu_t *pcpu);

void disasm(cpu_t *pcpu);
