#include "cpu.h"

#include "m68k.h"

static inline void write32(uint8_t *p, uint32_t data) {
    p[0] = data >> 24;
    p[1] = (data >> 16) & 0xff;
    p[2] = (data >> 8) & 0xff;
    p[3] = data & 0xff;
}

static inline void writeAddr16(uint8_t *p, uint16_t vaddr) {
    p[0] = 0;
    p[1] = 0;
    p[2] = vaddr >> 8;
    p[3] = vaddr & 0xff;
}

// TODO: 32bit
// TODO: virtual memory page をまたぐと動かない
// TODO: machine 側に持っていくべきか？
uint16_t pushArgs(cpu_t *pcpu, int argc, uint8_t *args, size_t argsbytes) {
    uint32_t sp = m68k_get_reg(NULL, M68K_REG_SP);

    // argc, argv[0]...argv[na-1], -1, buf
    const uint32_t na = argc;
    const uint16_t vsp = sp - (4 + na * 4 + 4 + argsbytes);
    uint8_t *rsp = pcpu->mmuV2R(pcpu->ctx, vsp);
    uint8_t *pbuf = rsp + 4 + na * 4 + 4;

    // argc
    write32(rsp, na);
    rsp += 4;

    // argv & buf
    const uint8_t *pa = args;
    for (int i = 0; i < na; i++) {
        uint16_t vaddr = pcpu->mmuR2V(pcpu->ctx, pbuf);
        writeAddr16(rsp, vaddr);
        rsp += 4;
        do {
            *pbuf++ = *pa;
        } while (*pa++ != '\0');
    }

    uint16_t vaddr = pcpu->mmuR2V(pcpu->ctx, pbuf);
    if (vaddr & 1) {
        *pbuf = '\0'; // alignment
    }

    // -1
    write32(rsp, 0xffffffff);

    m68k_set_reg(M68K_REG_SP, vsp);
    return vsp;
}
