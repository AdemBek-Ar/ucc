/* ========================================================================== *
||   Token file: hold the Token struct (Tk) and relatives                    ||
||        *TkT        : Token Type, 1byte id                                 ||
||        *tkls       : Token Text List                                      ||
||        *tkf_##kind : Token Kind selectors, fisrt                          ||
||        *tkl_##kind : Token Kind selectors, last                           ||
||        *istk_##kind: function check if Token is in Kind                   ||
||        *OpHashPair : struct hold hash key and TkT id for an operation     ||
||        *minihash   : function hashing an operation text                   ||
||        *kind##_lookup: operation lookup table                             ||
||        *Err        : struct for holding error                             ||
||        *Tk         : the main Token struct                                ||
||        *Tk::Wrap   : Token-additional-data Wrap struct                    ||
 * ========================================================================== */

#define CYVER_F_KIND(kind,NAME) // X-Macros help in database reading
#define CYVER_L_KIND(kind,NAME)

enum TkT /* 1byte */ {
	#define CYVER_TOKEN(_,NAME) NAME,
	#include "../../db/tokens/cyv.h"
	#undef CYVER_TOKEN
};

constexpr std::string_view tkls[]= /* tokens list */ {
	#define CYVER_TOKEN(txt,_) txt,
	#include "../../db/tokens/cyv.h"
	#undef CYVER_TOKEN
};
#undef CYVER_F_KIND
#undef CYVER_L_KIND


#define CYVER_TOKEN(_,__)
#define CYVER_F_KIND(kind,NAME) constexpr TkT tkf_##kind=TkT::NAME; // first token in kind
#define CYVER_L_KIND(kind,NAME) constexpr TkT tkl_##kind=TkT::NAME; // last token out kind
#include "../../db/tokens/cyv.h"
#undef CYVER_F_KIND
#undef CYVER_L_KIND

// kind check function
#define CYVER_F_KIND(kind,_) inline bool istk_##kind(TkT id) { return id>=tkf_##kind && id<tkl_##kind; }
#define CYVER_L_KIND(_,__)
#include "../../db/tokens/cyv.h"
#undef CYVER_F_KIND
#undef CYVER_L_KIND
#undef CYVER_TOKEN



inline bool istk_blk(TkT id) { return id>=TkT::LB && id<TkT::BSH; }


// needed for operations text hashing
struct OpHashPair /* 8byte */ {
	u32 hash;
	TkT id;
	// 3byte left
};

constexpr u32 minihash(std::string_view str) {
    u32 hash = 0;
    for (usz i=0; i<str.size() && i<4; ++i)
        hash |= (u32(str[i]) << (i * 8));
    return hash;
}

// operation lookup table
#define CYVER_F_KIND(kind, __)   constexpr OpHashPair kind##_lookup[] = {
#define CYVER_TOKEN(VAR, ID)     	{ minihash(VAR), TkT::ID },
#define CYVER_L_KIND(kind, __)   };
#include "../../db/tokens/cyvop.h"
#undef CYVER_F_KIND
#undef CYVER_L_KIND
#undef CYVER_TOKEN



struct Err /* 32byte */ {
	std::string_view msg;
	usz loc, len;
};



struct Tk /* 16byte */ {
	u32 loc; // location of token in 4GB file
	u16 len; // lenght of token, can carry to 64KB of lenght
	union {
		TkT id; // token id
		u8 n=0;
	};
    u8 fid; // file id
	union Wrap {
		struct { u16 rgid, rgsz; }; // register literal info
		std::vector<Tk> *block;     // if token is block
		std::string *txt;           // *text            in symbols table
		gmp::fbig *f;               // *big float value in floats table
		gmp::ibig *i;               // *big int   value in integers table
		ch32 c;                     // UTF8-UTF32 char value
		// bool b;
	} data;

    
	inline Tk(TkT id=TkT::UNK, u32 loc=0, u16 len=0, Wrap data={.c=0})
		:loc(loc), len(len), id(id), data(data) {}


	inline operator bool() { return id!=TkT::_EOF; } // for quick iteration


	#ifdef DBG // only on debug mode
		inline void print(u16 tabs=0) {
			for (u16 i=0; i<tabs; i++) std::putchar('\t');
			std::print("[{:<4} {:<5} {:<4}]", tkls[n], loc, len);
			switch (id) {
				case TkT::INCLUDE:
				case TkT::TXT_LIT:
				case TkT::IDENT:     std::print("=`{}`", *data.txt);         break;
				// case TkT::BOOL_LIT:  std::print("=`{}`", data.b);         break;
				case TkT::NUM_LIT:   std::print("={}", data.i->get_str(10)); break;
				case TkT::RG_LIT:    std::print("=${}x{}", data.rgid, data.rgsz);              break;
				case TkT::CHAR_LIT:  std::print("=`{}`", static_cast<char[4]>(int(data.c)));   break;
				case TkT::FLOAT_LIT:
					gmp::exp exp;
					std::print("={}", data.f->get_str(exp, 10));
					break;
				case TkT::LB:
				case TkT::LBR:
				case TkT::LPR:
				case TkT::TAB:
				case TkT::TKWOWT:
					std::putchar('\n');
					if (data.block)
						for (auto &tk: *data.block) tk.print(tabs+1);
					break;
				default: break;
			}
			std::putchar('\n');
		}
	#endif
};



using TksView=std::span<Tk>;
using Tks=std::vector<Tk>;
