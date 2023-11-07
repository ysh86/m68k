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
    if (address < 0x00000400) {
        printf("/ exception %d\n", address>>2);
        printf("/ fetch 32: addr=%08x\n"
            "/ pc=%08x, sr=%04x, sp=%08x, usp=%08x, isp=%08x\n"
            "/  d: %08x, %08x, %08x, %08x, %08x, %08x, %08x, %08x\n"
            "/  a: %08x, %08x, %08x, %08x, %08x, %08x, %08x, %08x\n",
            address,
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
        for (uint32_t addr = m68k_get_reg(NULL, M68K_REG_ISP); addr < 0x0400; addr += 2) {
            uint16_t *ra16 = (uint16_t *)(theCPU->mmuV2R(theCPU->ctx, addr));
            printf("/    %08x: %04x\n", addr, ntohs(*ra16));
        }

        // TODO
        //theCPU->syscallHook(theCPU->ctx)
        exit(-1);
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
