#include <stdio.h>

#include "m68k.h"

#include "cpu.h"
#include "util.h"

void init(
    cpu_t *pcpu,
    context_t ctx,
    mmu_v2r_t v2r,
    mmu_r2v_t r2v,
    syscall_t syscallHook,
    syscall_string_t syscallStringHook,
    uint16_t sp, uint16_t pc) {
    // support only one cpu
    theCPU = pcpu;

    // machine
    pcpu->ctx = ctx;
    pcpu->mmuV2R = v2r;
    pcpu->mmuR2V = r2v;
    pcpu->syscallHook = syscallHook;
    pcpu->syscallStringHook = syscallStringHook;

    // core
    m68k_set_cpu_type(M68K_CPU_TYPE_68020);
    m68k_init();
    m68k_pulse_reset();

    m68k_set_reg(M68K_REG_SR, 0x2000); // supervisor mode
    m68k_set_reg(M68K_REG_USP, sp);
    m68k_set_reg(M68K_REG_SR, 0x0000); // user mode
    m68k_set_reg(M68K_REG_SP, sp);
    m68k_set_reg(M68K_REG_ISP, SIZE_OF_VECTORS); // for exception
    m68k_set_reg(M68K_REG_PPC, pc); // for disasm
    m68k_set_reg(M68K_REG_PC, pc);
    pcpu->pc = pc; // TODO: for syscall
    pcpu->sp = sp; // TODO: debug
}

uint16_t fetch(cpu_t *pcpu) {
    // dummy
    return 0;
}

void decode(cpu_t *pcpu) {
    // dummy
}

void exec(cpu_t *pcpu) {
    m68k_execute(1);
    pcpu->pc = m68k_get_reg(NULL, M68K_REG_PC); // TODO: for syscall
    pcpu->sp = m68k_get_reg(NULL, M68K_REG_SP); // TODO: debug
}

void disasm(cpu_t *pcpu) {
    uint32_t ppc = m68k_get_reg(NULL, M68K_REG_PPC);
    char str[1024];
    m68k_disassemble(str, ppc, m68k_get_reg(NULL, M68K_REG_CPU_TYPE));
    printf("/ %08x: %s\n", ppc, str);
#if 0
    printf("/ pc=%08x, sr=%04x, sp=%08x, usp=%08x, isp=%08x\n"
        "/  d: %08x, %08x, %08x, %08x, %08x, %08x, %08x, %08x\n"
        "/  a: %08x, %08x, %08x, %08x, %08x, %08x, %08x, %08x\n",
        ppc,
        m68k_get_reg(NULL, M68K_REG_SR),
        m68k_get_reg(NULL, M68K_REG_SP),
        m68k_get_reg(NULL, M68K_REG_USP),
        m68k_get_reg(NULL, M68K_REG_ISP),
        m68k_get_reg(NULL, M68K_REG_D0),
        m68k_get_reg(NULL, M68K_REG_D1),
        m68k_get_reg(NULL, M68K_REG_D2),
        m68k_get_reg(NULL, M68K_REG_D3),
        m68k_get_reg(NULL, M68K_REG_D4),
        m68k_get_reg(NULL, M68K_REG_D5),
        m68k_get_reg(NULL, M68K_REG_D6),
        m68k_get_reg(NULL, M68K_REG_D7),
        m68k_get_reg(NULL, M68K_REG_A0),
        m68k_get_reg(NULL, M68K_REG_A1),
        m68k_get_reg(NULL, M68K_REG_A2),
        m68k_get_reg(NULL, M68K_REG_A3),
        m68k_get_reg(NULL, M68K_REG_A4),
        m68k_get_reg(NULL, M68K_REG_A5),
        m68k_get_reg(NULL, M68K_REG_A6),
        m68k_get_reg(NULL, M68K_REG_A7)
    );
#endif

    // TODO
    //pcpu->syscallStringHook()
}
