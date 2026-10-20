#include "elf.h++"


struct Gen {
    std::vector<u8> bin;
    std::string output="a.out";
    ELFHeader elf;
    std::set<std::string> sll, dll;
    enum OutLang { BIN=0, ASM=1, WASM=2, QBE=3, LLVM=4, C=5, CPP=6, RUST=7 };
    ArchId arch;
    bool dbg:1=0, clean:1=0, shared:1=0, opt:1=0;
    OutLang lang:3=C;
    // 41bit left

    inline void emit(std::string txt) { for (char c:txt) emit(u8(c)); }
    inline void emit(ch8 b) { bin.push_back((u8)b); }
    inline void emit(u8 b)  { bin.push_back(b); }
    inline void emit(u32 val) {
        union {
            u32 val;
            u8 data[sizeof(u32)];
        } v; v.val=val;
        for (u8 i=1; i<sizeof(u32); i++)
            emit(u8((v.data[i] >> (i*8))& 0xFF));
    }
    inline void emit(u64 val) {
        union {
            u64 val;
            u8 data[sizeof(u64)];
        } v; v.val=val;
        for (u8 i=1; i<sizeof(u64); i++)
            emit(u8((v.data[i] >> (i*8))& 0xFF));
    }



    #include "./generator/C.h++"



    inline int gen(Parser *parser) {
        switch (lang) {
            case C:   return genc(parser);
            // case ASM: return genasm(parser);
            // case CPP: return gencpp(parser);
            // case BIN: return genbin(parser);
            default: perr("unsupported langouge");
        }
        return 1;
    }



    inline int out() {
        std::filesystem::path p(output);
        if (p.has_parent_path())
            std::filesystem::create_directories(p.parent_path());
        std::FILE *file=std::fopen(output.c_str(), "w");
        if (!file) {
            perr("failed to save output");
            return -1;
        }
        fwrite(bin.data(), bin.size(), 1, file);
        std::fclose(file);
        return 0;
    }
};
