#include "cpu.h"

#include "m68k.h"

static inline void write16(uint8_t *p, uint16_t data) {
    p[0] = data >> 8;
    p[1] = data & 0xff;
}

// TODO: 32bit
// TODO: virtual memory page をまたぐと動かない
// TODO: machine 側に持っていくべきか？
uint16_t pushArgs(cpu_t *pcpu, int argc, uint8_t *args, size_t argsbytes) {
    uint32_t sp = m68k_get_reg(NULL, M68K_REG_SP);

    // argc, argv[0]...argv[na-1], -1, buf
    const uint16_t na = argc;
    const uint16_t vsp = sp - (2 + na * 2 + 2 + argsbytes);
    uint8_t *rsp = pcpu->mmuV2R(pcpu->ctx, vsp);
    uint8_t *pbuf = rsp + 2 + na * 2 + 2;

    // argc
    write16(rsp, na);
    rsp += 2;

    // argv & buf
    const uint8_t *pa = args;
    for (int i = 0; i < na; i++) {
        uint16_t vaddr = pcpu->mmuR2V(pcpu->ctx, pbuf);
        write16(rsp, vaddr);
        rsp += 2;
        do {
            *pbuf++ = *pa;
        } while (*pa++ != '\0');
    }

    uint16_t vaddr = pcpu->mmuR2V(pcpu->ctx, pbuf);
    if (vaddr & 1) {
        *pbuf = '\0'; // alignment
    }

    // -1
    write16(rsp, 0xffff);

    m68k_set_reg(M68K_REG_SP, vsp);
    return vsp;
}
