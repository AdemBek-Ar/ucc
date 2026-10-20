#include "./arch.h++"

struct SHeader {
	enum struct Flags: u32 { W=1, A=2, X=4,  MASKPROC=0xF0000000 };
	enum struct Kind : u32 {
		NON=0, PROGBITS=1, SYMTAB=2, STR_TAB=3, RELA=4, REL=9,
		HASH=5, DYN=6, NOTE=7, NOBITS=8, SHLIB=10, DYNSYM=11,
		LOPROC=0x70000000, HIPROC=0x7FFFFFFF,
		LOUSER=0x80000000, HIUSER=0xFFFFFFFF
	};
	usz   name=0;
	Kind  type=Kind::NON;
	u32   flags=0;
	usz   addr=0, offset=0;
	usz   sz=0,   link=0, info=0,
	addralign=0, entsz=0;
};



struct PHeader_x32 {
	enum struct PF  : u32 { X=1, W, R=6}; // Promition
	enum struct Kind: u32 {
		NON=0, LOAD=1, DYN=2, INTP=3, NOTE=4, SHLIB=5, PHDR=6, TLS=7,
		LOOS=0x60000000, HIOS=0x6FFFFFFF
	};
	Kind type=Kind::LOAD;
	usz  offset=0,
	vaddr=0, paddr=0;
	u32  fsz=0,   memsz=0;
	u32   flags=u32(PF::R)|u32(PF::X);
	u32  align=0;
};



struct PHeader_x64 { // rewrite a Struct just because of `offset` position
	using Kind=PHeader_x32::Kind;
	using PF=PHeader_x32::PF;

	Kind type=Kind::LOAD;
	u32   flags=u32(PF::R)|u32(PF::X);
	usz  offset=0,
	vaddr=0, paddr=0;
	u64  fsz=0,   memsz=0;
	u64  align=0;
};



struct ELFHeader {
	enum struct Kind : u16 { NON=0, OBJ=1, EXE=2, DYN=3, CORE=4 };
	struct IdentStruct /* 16byte */ {
		enum struct Os   : u8 { NON=0, HP_UX=1, NetBSD=2, Linux=3, Unix=4, Mac=5, Window=6 };
		enum struct Kind : u8 { INVL=0, x32=1, x64=2 };
		enum struct Fbyt : u8 { NON=0,  LSB=1, MSB=2 };
		union {
			u8  marr[4]= {0x7f, 'E', 'L', 'F'};
			u32 mnum;
		};
		Kind kind     =Kind::x64;
		Fbyt order    =Fbyt::LSB;
		u8   version  =1;
		Os   os       =Os::NON;
		u8   ABI_version=0;
		u8   padding[7]={0}; // will be used to handle package stuff

		inline IdentStruct()=default;
	};

	// ph(t): program header (table)
	// sh(t): section header (table)

	union Ident {
		IdentStruct infoed;
		u8 e_ident[16];

		inline Ident() :infoed() {}
	};
	Ident  ident;
	Kind   type=Kind::EXE;
	ArchId arch=ArchId::x86;
	u32    version=1;
	usz    entry=0,
	phoff=sizeof(ELFHeader),  shoff=0;
	u32    flags=0;
	u16    size=sizeof(ELFHeader),
	ph_size=0,  pht_len=1,
	sh_size=sizeof(SHeader),  sht_len=0,
	shstrndx=0;

	inline ELFHeader()=default;
};
