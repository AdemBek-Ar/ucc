/* ========================================================================== */
/* x86-64 Architecture Registers Definition File (.h)                         */
/* ========================================================================== */

#ifndef RGST
#define RGST(str_lower, enum_id, str_upper, enc_id, size_bytes, reg_class)
#endif

/* -------------------------------------------------------------------------- */
/* 1. General Purpose Registers (GPRs) - 64-bit                               */
/* -------------------------------------------------------------------------- */
RGST("rax", RAX, "RAX", 0,  8, GPR64)
RGST("rcx", RCX, "RCX", 1,  8, GPR64)
RGST("rdx", RDX, "RDX", 2,  8, GPR64)
RGST("rbx", RBX, "RBX", 3,  8, GPR64)
RGST("rsp", RSP, "RSP", 4,  8, GPR64)
RGST("rbp", RBP, "RBP", 5,  8, GPR64)
RGST("rsi", RSI, "RSI", 6,  8, GPR64)
RGST("rdi", RDI, "RDI", 7,  8, GPR64)
RGST("r8",  R8,  "R8",  8,  8, GPR64)
RGST("r9",  R9,  "R9",  9,  8, GPR64)
RGST("r10", R10, "R10", 10, 8, GPR64)
RGST("r11", R11, "R11", 11, 8, GPR64)
RGST("r12", R12, "R12", 12, 8, GPR64)
RGST("r13", R13, "R13", 13, 8, GPR64)
RGST("r14", R14, "R14", 14, 8, GPR64)
RGST("r15", R15, "R15", 15, 8, GPR64)

/* -------------------------------------------------------------------------- */
/* 2. General Purpose Registers (Sub-registers) - 32-bit                      */
/* -------------------------------------------------------------------------- */
RGST("eax", EAX, "EAX", 0,  4, GPR32)
RGST("ecx", ECX, "ECX", 1,  4, GPR32)
RGST("edx", EDX, "EDX", 2,  4, GPR32)
RGST("ebx", EBX, "EBX", 3,  4, GPR32)
RGST("esp", ESP, "ESP", 4,  4, GPR32)
RGST("ebp", EBP, "EBP", 5,  4, GPR32)
RGST("esi", ESI, "ESI", 6,  4, GPR32)
RGST("edi", EDI, "EDI", 7,  4, GPR32)
RGST("r8d", R8D, "R8D", 8,  4, GPR32)
RGST("r9d", R9D, "R9D", 9,  4, GPR32)
RGST("r10d", R10D, "R10D", 10, 4, GPR32)
RGST("r11d", R11D, "R11D", 11, 4, GPR32)
RGST("r12d", R12D, "R12D", 12, 4, GPR32)
RGST("r13d", R13D, "R13D", 13, 4, GPR32)
RGST("r14d", R14D, "R14D", 14, 4, GPR32)
RGST("r15d", R15D, "R15D", 15, 4, GPR32)

/* -------------------------------------------------------------------------- */
/* 3. General Purpose Registers (Sub-registers) - 16-bit                      */
/* -------------------------------------------------------------------------- */
RGST("ax",   AX,   "AX",   0,  2, GPR16)
RGST("cx",   CX,   "CX",   1,  2, GPR16)
RGST("dx",   DX,   "DX",   2,  2, GPR16)
RGST("bx",   BX,   "BX",   3,  2, GPR16)
RGST("sp",   SP,   "SP",   4,  2, GPR16)
RGST("bp",   BP,   "BP",   5,  2, GPR16)
RGST("si",   SI,   "SI",   6,  2, GPR16)
RGST("di",   DI,   "DI",   7,  2, GPR16)
RGST("r8w",  R8W,  "R8W",  8,  2, GPR16)
RGST("r9w",  R9W,  "R9W",  9,  2, GPR16)
RGST("r10w", R10W, "R10W", 10, 2, GPR16)
RGST("r11w", R11W, "R11W", 11, 2, GPR16)
RGST("r12w", R12W, "R12W", 12, 2, GPR16)
RGST("r13w", R13W, "R13W", 13, 2, GPR16)
RGST("r14w", R14W, "R14W", 14, 2, GPR16)
RGST("r15w", R15W, "R15W", 15, 2, GPR16)

/* -------------------------------------------------------------------------- */
/* 4. General Purpose Registers (Sub-registers) - 8-bit Low                   */
/* -------------------------------------------------------------------------- */
RGST("al",   AL,   "AL",   0,  1, GPR8L)
RGST("cl",   CL,   "CL",   1,  1, GPR8L)
RGST("dl",   DL,   "DL",   2,  1, GPR8L)
RGST("bl",   BL,   "BL",   3,  1, GPR8L)
RGST("spl",  SPL,  "SPL",  4,  1, GPR8L)
RGST("bpl",  BPL,  "BPL",  5,  1, GPR8L)
RGST("sil",  SIL,  "SIL",  6,  1, GPR8L)
RGST("dil",  DIL,  "DIL",  7,  1, GPR8L)
RGST("r8b",  R8B,  "R8B",  8,  1, GPR8L)
RGST("r9b",  R9B,  "R9B",  9,  1, GPR8L)
RGST("r10b", R10B, "R10B", 10, 1, GPR8L)
RGST("r11b", R11B, "R11B", 11, 1, GPR8L)
RGST("r12b", R12B, "R12B", 12, 1, GPR8L)
RGST("r13b", R13B, "R13B", 13, 1, GPR8L)
RGST("r14b", R14B, "R14B", 14, 1, GPR8L)
RGST("r15b", R15B, "R15B", 15, 1, GPR8L)

/* -------------------------------------------------------------------------- */
/* 5. General Purpose Registers (Legacy Sub-registers) - 8-bit High           */
/* -------------------------------------------------------------------------- */
RGST("ah", AH, "AH", 4, 1, GPR8H)
RGST("ch", CH, "CH", 5, 1, GPR8H)
RGST("dh", DH, "DH", 6, 1, GPR8H)
RGST("bh", BH, "BH", 7, 1, GPR8H)

/* -------------------------------------------------------------------------- */
/* 6. Instruction Pointer & Flags                                             */
/* -------------------------------------------------------------------------- */
RGST("rip",    RIP,    "RIP",    0, 8, IP)
RGST("eip",    EIP,    "EIP",    0, 4, IP)
RGST("ip",     IP,     "IP",     0, 2, IP)
RGST("rflags", RFLAGS, "RFLAGS", 0, 8, FLAGS)
RGST("eflags", EFLAGS, "EFLAGS", 0, 4, FLAGS)
RGST("flags",  FLAGS,  "FLAGS",  0, 2, FLAGS)

/* -------------------------------------------------------------------------- */
/* 7. Segment Registers                                                       */
/* -------------------------------------------------------------------------- */
RGST("es", ES, "ES", 0, 2, SEGMENT)
RGST("cs", CS, "CS", 1, 2, SEGMENT)
RGST("ss", SS, "SS", 2, 2, SEGMENT)
RGST("ds", DS, "DS", 3, 2, SEGMENT)
RGST("fs", FS, "FS", 4, 2, SEGMENT)
RGST("gs", GS, "GS", 5, 2, SEGMENT)

/* -------------------------------------------------------------------------- */
/* 8. x87 FPU Registers (80-bit Floating Point)                               */
/* -------------------------------------------------------------------------- */
RGST("st0", ST0, "ST0", 0, 10, FPU)
RGST("st1", ST1, "ST1", 1, 10, FPU)
RGST("st2", ST2, "ST2", 2, 10, FPU)
RGST("st3", ST3, "ST3", 3, 10, FPU)
RGST("st4", ST4, "ST4", 4, 10, FPU)
RGST("st5", ST5, "ST5", 5, 10, FPU)
RGST("st6", ST6, "ST6", 6, 10, FPU)
RGST("st7", ST7, "ST7", 7, 10, FPU)

/* -------------------------------------------------------------------------- */
/* 9. MMX Registers (64-bit, Aliased on x87 FPU)                              */
/* -------------------------------------------------------------------------- */
RGST("mm0", MM0, "MM0", 0, 8, MMX)
RGST("mm1", MM1, "MM1", 1, 8, MMX)
RGST("mm2", MM2, "MM2", 2, 8, MMX)
RGST("mm3", MM3, "MM3", 3, 8, MMX)
RGST("mm4", MM4, "MM4", 4, 8, MMX)
RGST("mm5", MM5, "MM5", 5, 8, MMX)
RGST("mm6", MM6, "MM6", 6, 8, MMX)
RGST("mm7", MM7, "MM7", 7, 8, MMX)

/* -------------------------------------------------------------------------- */
/* 10. AVX-512 Opmask Registers                                               */
/* -------------------------------------------------------------------------- */
RGST("k0", K0, "K0", 0, 8, MASK)
RGST("k1", K1, "K1", 1, 8, MASK)
RGST("k2", K2, "K2", 2, 8, MASK)
RGST("k3", K3, "K3", 3, 8, MASK)
RGST("k4", K4, "K4", 4, 8, MASK)
RGST("k5", K5, "K5", 5, 8, MASK)
RGST("k6", K6, "K6", 6, 8, MASK)
RGST("k7", K7, "K7", 7, 8, MASK)

/* -------------------------------------------------------------------------- */
/* 11. Control Registers                                                      */
/* -------------------------------------------------------------------------- */
RGST("cr0", CR0, "CR0", 0, 8, CONTROL)
RGST("cr2", CR2, "CR2", 2, 8, CONTROL)
RGST("cr3", CR3, "CR3", 3, 8, CONTROL)
RGST("cr4", CR4, "CR4", 4, 8, CONTROL)
RGST("cr8", CR8, "CR8", 8, 8, CONTROL)

/* -------------------------------------------------------------------------- */
/* 12. Debug Registers                                                        */
/* -------------------------------------------------------------------------- */
RGST("dr0", DR0, "DR0", 0, 8, DEBUG)
RGST("dr1", DR1, "DR1", 1, 8, DEBUG)
RGST("dr2", DR2, "DR2", 2, 8, DEBUG)
RGST("dr3", DR3, "DR3", 3, 8, DEBUG)
RGST("dr6", DR6, "DR6", 6, 8, DEBUG)
RGST("dr7", DR7, "DR7", 7, 8, DEBUG)

/* -------------------------------------------------------------------------- */
/* 13. Vector Registers (SSE/AVX/AVX-512)                                    */
/* Note: XMM/YMM are lower parts of ZMM in x86-64                         */
/* -------------------------------------------------------------------------- */

/* 128-bit XMM Registers (0-31) */
RGST("xmm0",  XMM0,  "XMM0",  0,  16, XMM)
RGST("xmm1",  XMM1,  "XMM1",  1,  16, XMM)
RGST("xmm2",  XMM2,  "XMM2",  2,  16, XMM)
RGST("xmm3",  XMM3,  "XMM3",  3,  16, XMM)
RGST("xmm4",  XMM4,  "XMM4",  4,  16, XMM)
RGST("xmm5",  XMM5,  "XMM5",  5,  16, XMM)
RGST("xmm6",  XMM6,  "XMM6",  6,  16, XMM)
RGST("xmm7",  XMM7,  "XMM7",  7,  16, XMM)
RGST("xmm8",  XMM8,  "XMM8",  8,  16, XMM)
RGST("xmm9",  XMM9,  "XMM9",  9,  16, XMM)
RGST("xmm10", XMM10, "XMM10", 10, 16, XMM)
RGST("xmm11", XMM11, "XMM11", 11, 16, XMM)
RGST("xmm12", XMM12, "XMM12", 12, 16, XMM)
RGST("xmm13", XMM13, "XMM13", 13, 16, XMM)
RGST("xmm14", XMM14, "XMM14", 14, 16, XMM)
RGST("xmm15", XMM15, "XMM15", 15, 16, XMM)
RGST("xmm16", XMM16, "XMM16", 16, 16, XMM)
RGST("xmm17", XMM17, "XMM17", 17, 16, XMM)
RGST("xmm18", XMM18, "XMM18", 18, 16, XMM)
RGST("xmm19", XMM19, "XMM19", 19, 16, XMM)
RGST("xmm20", XMM20, "XMM20", 20, 16, XMM)
RGST("xmm21", XMM21, "XMM21", 21, 16, XMM)
RGST("xmm22", XMM22, "XMM22", 22, 16, XMM)
RGST("xmm23", XMM23, "XMM23", 23, 16, XMM)
RGST("xmm24", XMM24, "XMM24", 24, 16, XMM)
RGST("xmm25", XMM25, "XMM25", 25, 16, XMM)
RGST("xmm26", XMM26, "XMM26", 26, 16, XMM)
RGST("xmm27", XMM27, "XMM27", 27, 16, XMM)
RGST("xmm28", XMM28, "XMM28", 28, 16, XMM)
RGST("xmm29", XMM29, "XMM29", 29, 16, XMM)
RGST("xmm30", XMM30, "XMM30", 30, 16, XMM)
RGST("xmm31", XMM31, "XMM31", 31, 16, XMM)

/* 256-bit YMM Registers (0-31) */
RGST("ymm0",  YMM0,  "YMM0",  0,  32, YMM)
RGST("ymm1",  YMM1,  "YMM1",  1,  32, YMM)
RGST("ymm2",  YMM2,  "YMM2",  2,  32, YMM)
RGST("ymm3",  YMM3,  "YMM3",  3,  32, YMM)
RGST("ymm4",  YMM4,  "YMM4",  4,  32, YMM)
RGST("ymm5",  YMM5,  "YMM5",  5,  32, YMM)
RGST("ymm6",  YMM6,  "YMM6",  6,  32, YMM)
RGST("ymm7",  YMM7,  "YMM7",  7,  32, YMM)
RGST("ymm8",  YMM8,  "YMM8",  8,  32, YMM)
RGST("ymm9",  YMM9,  "YMM9",  9,  32, YMM)
RGST("ymm10", YMM10, "YMM10", 10, 32, YMM)
RGST("ymm11", YMM11, "YMM11", 11, 32, YMM)
RGST("ymm12", YMM12, "YMM12", 12, 32, YMM)
RGST("ymm13", YMM13, "YMM13", 13, 32, YMM)
RGST("ymm14", YMM14, "YMM14", 14, 32, YMM)
RGST("ymm15", YMM15, "YMM15", 15, 32, YMM)
RGST("ymm16", YMM16, "YMM16", 16, 32, YMM)
RGST("ymm17", YMM17, "YMM17", 17, 32, YMM)
RGST("ymm18", YMM18, "YMM18", 18, 32, YMM)
RGST("ymm19", YMM19, "YMM19", 19, 32, YMM)
RGST("ymm20", YMM20, "YMM20", 20, 32, YMM)
RGST("ymm21", YMM21, "YMM21", 21, 32, YMM)
RGST("ymm22", YMM22, "YMM22", 22, 32, YMM)
RGST("ymm23", YMM23, "YMM23", 23, 32, YMM)
RGST("ymm24", YMM24, "YMM24", 24, 32, YMM)
RGST("ymm25", YMM25, "YMM25", 25, 32, YMM)
RGST("ymm26", YMM26, "YMM26", 26, 32, YMM)
RGST("ymm27", YMM27, "YMM27", 27, 32, YMM)
RGST("ymm28", YMM28, "YMM28", 28, 32, YMM)
RGST("ymm29", YMM29, "YMM29", 29, 32, YMM)
RGST("ymm30", YMM30, "YMM30", 30, 32, YMM)
RGST("ymm31", YMM31, "YMM31", 31, 32, YMM)

/* 512-bit ZMM Registers (0-31) */
RGST("zmm0",  ZMM0,  "ZMM0",  0,  64, ZMM)
RGST("zmm1",  ZMM1,  "ZMM1",  1,  64, ZMM)
RGST("zmm2",  ZMM2,  "ZMM2",  2,  64, ZMM)
RGST("zmm3",  ZMM3,  "ZMM3",  3,  64, ZMM)
RGST("zmm4",  ZMM4,  "ZMM4",  4,  64, ZMM)
RGST("zmm5",  ZMM5,  "ZMM5",  5,  64, ZMM)
RGST("zmm6",  ZMM6,  "ZMM6",  6,  64, ZMM)
RGST("zmm7",  ZMM7,  "ZMM7",  7,  64, ZMM)
RGST("zmm8",  ZMM8,  "ZMM8",  8,  64, ZMM)
RGST("zmm9",  ZMM9,  "ZMM9",  9,  64, ZMM)
RGST("zmm10", ZMM10, "ZMM10", 10, 64, ZMM)
RGST("zmm11", ZMM11, "ZMM11", 11, 64, ZMM)
RGST("zmm12", ZMM12, "ZMM12", 12, 64, ZMM)
RGST("zmm13", ZMM13, "ZMM13", 13, 64, ZMM)
RGST("zmm14", ZMM14, "ZMM14", 14, 64, ZMM)
RGST("zmm15", ZMM15, "ZMM15", 15, 64, ZMM)
RGST("zmm16", ZMM16, "ZMM16", 16, 64, ZMM)
RGST("zmm17", ZMM17, "ZMM17", 17, 64, ZMM)
RGST("zmm18", ZMM18, "ZMM18", 18, 64, ZMM)
RGST("zmm19", ZMM19, "ZMM19", 19, 64, ZMM)
RGST("zmm20", ZMM20, "ZMM20", 20, 64, ZMM)
RGST("zmm21", ZMM21, "ZMM21", 21, 64, ZMM)
RGST("zmm22", ZMM22, "ZMM22", 22, 64, ZMM)
RGST("zmm23", ZMM23, "ZMM23", 23, 64, ZMM)
RGST("zmm24", ZMM24, "ZMM24", 24, 64, ZMM)
RGST("zmm25", ZMM25, "ZMM25", 25, 64, ZMM)
RGST("zmm26", ZMM26, "ZMM26", 26, 64, ZMM)
RGST("zmm27", ZMM27, "ZMM27", 27, 64, ZMM)
RGST("zmm28", ZMM28, "ZMM28", 28, 64, ZMM)
RGST("zmm29", ZMM29, "ZMM29", 29, 64, ZMM)
RGST("zmm30", ZMM30, "ZMM30", 30, 64, ZMM)
RGST("zmm31", ZMM31, "ZMM31", 31, 64, ZMM)
