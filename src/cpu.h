#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define SIZE_OF_VECTORS 0x0400

typedef void * context_t;
typedef uint8_t *(*mmu_v2r_t)(context_t ctx, uint16_t vaddr); // TODO: 32bit
typedef uint16_t (*mmu_r2v_t)(context_t ctx, uint8_t *raddr); // TODO: 32bit
typedef void (*syscall_t)(context_t ctx);
typedef void (*syscall_string_t)(context_t ctx, char *str, size_t size, uint8_t id);

struct cpu_tag {
    // machine
    context_t ctx;
    mmu_v2r_t mmuV2R;
    mmu_r2v_t mmuR2V;
    syscall_t syscallHook;
    syscall_string_t syscallStringHook;

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
    syscall_string_t syscallStringHook,
    uint16_t sp, uint16_t pc); // TODO: 32bit
uint16_t pushArgs(cpu_t *pcpu, int argc, uint8_t *args, size_t argsbytes);

uint32_t getSP(cpu_t *pcpu);
uint32_t getPC(cpu_t *pcpu);
uint32_t rte(cpu_t *pcpu, uint16_t ustatus, uint32_t usp, uint32_t isp, uint32_t ppc, uint32_t pc);

uint16_t fetch(cpu_t *pcpu);
void decode(cpu_t *pcpu);
void exec(cpu_t *pcpu);

void disasm(cpu_t *pcpu);