#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <assert.h>

#include "util.h"

#include "m68k.h"

cpu_t *theCPU;

// Read from anywhere
unsigned int  m68k_read_memory_8(unsigned int address) {
    return *theCPU->mmuV2R(theCPU->ctx, address);
}
unsigned int  m68k_read_memory_16(unsigned int address) {
    uint8_t hi = *theCPU->mmuV2R(theCPU->ctx, address);
    uint8_t lo = *theCPU->mmuV2R(theCPU->ctx, address+1);
    return ((hi << 8) | lo);
}
unsigned int  m68k_read_memory_32(unsigned int address) {
    if (address < SIZE_OF_VECTORS) {
        uint32_t no = address >> 2;
#if 0
        fprintf(stderr, "/ %08x: exception %d\n", address, no);
        fprintf(stderr, "/ pc=%08x, sr=%04x, sp=%08x, usp=%08x, isp=%08x\n"
            "/  d: %08x, %08x, %08x, %08x, %08x, %08x, %08x, %08x\n"
            "/  a: %08x, %08x, %08x, %08x, %08x, %08x, %08x, %08x\n",
            m68k_get_reg(NULL, M68K_REG_PC),
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
        for (uint32_t addr = m68k_get_reg(NULL, M68K_REG_ISP); addr < SIZE_OF_VECTORS; addr += 2) {
            uint16_t *ra16 = (uint16_t *)(theCPU->mmuV2R(theCPU->ctx, addr));
            fprintf(stderr, "/    %08x: %04x\n", addr, ntohs(*ra16));
        }
#endif

        if (no == 32) {
            // trap
            uint16_t sr = m68k_get_reg(NULL, M68K_REG_SR);
            assert((sr & 0x3000) == 0x2000); // supervisor mode

            theCPU->syscallHook(theCPU->ctx);

            // rte manually
            uint32_t isp = m68k_get_reg(NULL, M68K_REG_SP);
            assert((isp & 1) == 0); // isp is word-aligned.
            sr = ntohs(*(uint16_t *)(theCPU->mmuV2R(theCPU->ctx, isp)));
            isp += 2;
            uint16_t pch = ntohs(*(uint16_t *)(theCPU->mmuV2R(theCPU->ctx, isp)));
            isp += 2;
            uint16_t pcl = ntohs(*(uint16_t *)(theCPU->mmuV2R(theCPU->ctx, isp)));
            isp += 2;
            uint16_t offset = ntohs(*(uint16_t *)(theCPU->mmuV2R(theCPU->ctx, isp)));
            isp += 2;
            m68k_set_reg(M68K_REG_SP, isp);

            uint32_t pc = ((pch<<16) | pcl);
            uint32_t usp = m68k_get_reg(NULL, M68K_REG_USP);

            // return to user mode
            assert((sr & 0x3000) == 0);
            assert((offset&0xf000) == 0);
            assert((offset&0x0fff) == 0x80);
            return rte(theCPU, sr, usp, isp, pc-2, pc);
        } else {
            fprintf(stderr, "exception %d is not implemented\n", no);
            exit(-1);
        }
    }

    uint8_t b24 = *theCPU->mmuV2R(theCPU->ctx, address);
    uint8_t b16 = *theCPU->mmuV2R(theCPU->ctx, address+1);
    uint8_t b8 = *theCPU->mmuV2R(theCPU->ctx, address+2);
    uint8_t b0 = *theCPU->mmuV2R(theCPU->ctx, address+3);
    return ((b24 << 24) | (b16 << 16) | (b8 << 8) | b0);
}

// Read data immediately following the PC
unsigned int  m68k_read_immediate_16(unsigned int address) {
    return m68k_read_memory_16(address);
}
unsigned int  m68k_read_immediate_32(unsigned int address) {
    return m68k_read_memory_32(address);
}

// Read data relative to the PC
unsigned int  m68k_read_pcrelative_8(unsigned int address) {
    return m68k_read_memory_8(address);
}
unsigned int  m68k_read_pcrelative_16(unsigned int address) {
    return m68k_read_memory_16(address);
}
unsigned int  m68k_read_pcrelative_32(unsigned int address) {
    return m68k_read_memory_32(address);
}

// Memory access for the disassembler
unsigned int m68k_read_disassembler_8  (unsigned int address) {
    return m68k_read_memory_8(address);
}
unsigned int m68k_read_disassembler_16 (unsigned int address) {
    return m68k_read_memory_16(address);
}
unsigned int m68k_read_disassembler_32 (unsigned int address) {
    return m68k_read_memory_32(address);
}

// Write to anywhere
void m68k_write_memory_8(unsigned int address, unsigned int value) {
    *theCPU->mmuV2R(theCPU->ctx, address) = value & 0xff;
}
void m68k_write_memory_16(unsigned int address, unsigned int value) {
    *theCPU->mmuV2R(theCPU->ctx, address) = (value >> 8) & 0xff;
    *theCPU->mmuV2R(theCPU->ctx, address+1) = value & 0xff;
}
void m68k_write_memory_32(unsigned int address, unsigned int value) {
    *theCPU->mmuV2R(theCPU->ctx, address) = (value >> 24) & 0xff;
    *theCPU->mmuV2R(theCPU->ctx, address+1) = (value >> 16) & 0xff;
    *theCPU->mmuV2R(theCPU->ctx, address+2) = (value >> 8) & 0xff;
    *theCPU->mmuV2R(theCPU->ctx, address+3) = value & 0xff;
}
