/* ========================================================================== *
 | Cyver Token Type (cyv::TkT) DataBase Table (.h)                            |
 | * stell have 138 place for other token types                               |
 |   but the last bit will be used to select the type of token type itself    |
 |   whether it is a C Token or a Cyv Token                                   |
 * ========================================================================== */

/*
CYVER_TOKEN(TEXT_IN_FILE,    NAME)
CYVER_F_KIND(KIND_NAME,    FIRST_TOKEN_IN_KIND)
CYVER_L_KIND(KIND_NAME,    LAST_TOKEN_IN_KIND+1)

 * the TkT that don't have a text reprisent them in the file
   are defined by empty string to not miss-indecis of thouse
   that have
 */
CYVER_TOKEN("UNK",              UNK)
CYVER_TOKEN("NLN",              NEW_LINE)

// literals
CYVER_F_KIND(lit,  CHAR_LIT)
	// CYVER_TOKEN("bool",  BOOL_LIT)
	CYVER_TOKEN("chr",   CHAR_LIT)    // 'c' 'أ' '🥱'
	CYVER_TOKEN("int",   NUM_LIT)     // 16  0h10  0o20 0b10000
	CYVER_TOKEN("flt",   FLOAT_LIT)   // 16.16  0h10.10  0o20.20 0b10000.10000
	CYVER_TOKEN("txt",   TXT_LIT)     // "text" 'text'
	CYVER_TOKEN("rgs",   RG_LIT)      // $0  $0x64  $s
CYVER_L_KIND(lit, FN)



// keywords
CYVER_F_KIND(kw, FN)
	CYVER_TOKEN("fn",     FN)
	CYVER_TOKEN("uni",    UNI)
	CYVER_TOKEN("use",    USE)
	CYVER_TOKEN("enm",    ENM)
	CYVER_TOKEN("typ",    TYP)
	CYVER_TOKEN("me",     ME)
	CYVER_TOKEN("let",    LET)
	CYVER_TOKEN("mod",    MOD)
	CYVER_TOKEN("pkg",    PKG)
	// CYVER_TOKEN("tag",    TAG)
	// CYVER_TOKEN("rpl",    RPL)
	// CYVER_TOKEN("non",    NON)
	// CYVER_TOKEN("op",     OP)
	// CYVER_TOKEN("T",      T)
	// CYVER_TOKEN("F",      F)

	// keywords fit as operators
	CYVER_F_KIND(opkw, RET)
		CYVER_TOKEN("ret",    RET)
		CYVER_TOKEN("do",     DO)
		CYVER_TOKEN("skp",    SKP)
		CYVER_TOKEN("brk",    BREAK)
		CYVER_TOKEN("if",     IF)
		CYVER_TOKEN("ef",     EF)
		CYVER_TOKEN("el",     EL)
		CYVER_TOKEN("lp",     LP)
		CYVER_TOKEN("get",    GET)
		CYVER_TOKEN("frm",    FRM)
		CYVER_TOKEN("in",     IN)
		CYVER_TOKEN("as",     AS)
		CYVER_TOKEN("to",     TO)
CYVER_L_KIND(kw, INT)



// assembly keywords
CYVER_F_KIND(usmkw, INT)
		CYVER_TOKEN("int",    INT)
		CYVER_TOKEN("mv",     MV)
		CYVER_TOKEN("jmp",    JMP)
		CYVER_TOKEN("cmp",    CMP)
		CYVER_TOKEN("sys",    SYS)
		// CYVER_TOKEN("lbl",    LBL)
		CYVER_TOKEN("psh",    PUSH)
		CYVER_TOKEN("pop",    POP)
	CYVER_L_KIND(opkw, BIN)
	CYVER_TOKEN("bin",    BIN)
	// CYVER_TOKEN("seg",    SEGMENT)
	// CYVER_TOKEN("sec",    SECTION)
	// CYVER_TOKEN("fmt",    FMT)
CYVER_L_KIND(usmkw, SIMICOLUMN)



// operators
#include "./cyvop.h"


CYVER_F_KIND(call, FNCALL)
	CYVER_TOKEN("FNC",              FNCALL)  // calling a fn,  arged-macro or const-fn
	CYVER_TOKEN("VRC",              VARCALL) // calling a var, macro or conat-var
CYVER_L_KIND(call, INCLUDE)

// more
CYVER_TOKEN("\\I",              INCLUDE) // \I "path", \I
CYVER_TOKEN("Id",               IDENT)   // ident scoped::ident
CYVER_TOKEN("BLK",              BLOCK)   // `([{\t}])`
CYVER_TOKEN("EOF",              _EOF)
