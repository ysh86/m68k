#include <stdio.h>
#include <assert.h>

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
    uint32_t sp, uint32_t pc) {
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

    // go to user mode
    rte(pcpu, 0x0000, sp, SIZE_OF_VECTORS, pc, pc);
}

uint32_t getISP(cpu_t *pcpu) {
    return m68k_get_reg(NULL, M68K_REG_ISP);
}
uint32_t getSP(cpu_t *pcpu) {
    return m68k_get_reg(NULL, M68K_REG_USP);
}
uint32_t getPC(cpu_t *pcpu) {
    return m68k_get_reg(NULL, M68K_REG_PC);
}
uint32_t getD0(cpu_t *pcpu) {
    return m68k_get_reg(NULL, M68K_REG_D0);
}
void setD0(cpu_t *pcpu, uint32_t d) {
    m68k_set_reg(M68K_REG_D0, d);
}
uint32_t getD1(cpu_t *pcpu) {
    return m68k_get_reg(NULL, M68K_REG_D1);
}
uint32_t getA0(cpu_t *pcpu) {
    return m68k_get_reg(NULL, M68K_REG_A0);
}

uint32_t rte(cpu_t *pcpu, uint16_t ustatus, uint32_t usp, uint32_t isp, uint32_t ppc, uint32_t pc) {
    uint16_t sr = m68k_get_reg(NULL, M68K_REG_SR);
    assert((sr & 0x3000) == 0x2000); // supervisor mode

    m68k_set_reg(M68K_REG_USP, usp);
    m68k_set_reg(M68K_REG_SR, ustatus); // user mode
    m68k_set_reg(M68K_REG_SP, usp);
    m68k_set_reg(M68K_REG_ISP, isp); // for exception
    m68k_set_reg(M68K_REG_PPC, ppc); // for disasm
    m68k_set_reg(M68K_REG_PC, pc);
#if 0
    fprintf(stderr, "/ rte manually: pc=%08x, sr=%04x, sp=%08x, usp=%08x, isp=%08x, a7=%08x\n",
        m68k_get_reg(NULL, M68K_REG_PC),
        m68k_get_reg(NULL, M68K_REG_SR),
        m68k_get_reg(NULL, M68K_REG_SP),
        m68k_get_reg(NULL, M68K_REG_USP),
        m68k_get_reg(NULL, M68K_REG_ISP),
        m68k_get_reg(NULL, M68K_REG_A7)
    );
#endif
    return pc;
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
}

void disasm(cpu_t *pcpu) {
    uint32_t pc = m68k_get_reg(NULL, M68K_REG_PC);
    fprintf(stderr, "/ %08x: ", pc);

    char str[1024];
    m68k_disassemble(str, pc, m68k_get_reg(NULL, M68K_REG_CPU_TYPE));
    fprintf(stderr, "%s\n", str);
#if 0
    fprintf(stderr, "/ pc=%08x, sr=%04x, sp=%08x, usp=%08x, isp=%08x\n"
        "/  d: %08x, %08x, %08x, %08x, %08x, %08x, %08x, %08x\n"
        "/  a: %08x, %08x, %08x, %08x, %08x, %08x, %08x, %08x\n",
        pc,
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
