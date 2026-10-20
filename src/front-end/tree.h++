/* ========================================================================== *
||   Tree file: hold the main AST structs                                    ||
||        *Code       : code holder data-struct                              ||
||        *Pkg        : package data-struct holder                           ||
||        *Typ        : type, union or enum data-struct                      ||
||        *Var        : var or macro data-struct holder                      ||
||        *Fn         : fn, op or macro data-struct holder                   ||
||        *Exp        : expretion data-struct holder                         ||
||        *TypHolder  : use to hold untyped literals                         ||
||        *Ident      : Identefire struct                                    ||
||        *Map        : main data struct to store Defines                    ||
||        *Def        : mapper Define with Ident                             ||
||        *Initls     : shortcut                                             ||
||        *Flags      : main Flags struct for all Typ, Var, Exp, Fn          ||
||        - these are explaining them selfs                                  ||
||        *Visit,Status,Consty,Tkind,Visit_ls,Status_ls,Consty_ls,Tkind_ls   ||
||        *is_expend, is_initop, is_def, is_op ,is_expblock, is_tydef        ||
 * ========================================================================== */


struct Pkg; // pkg
struct Typ; // typ, enm, uni
struct Var; // var, macro
struct Fn;  // fn, op, macro
struct Exp; // expretion, asm insteraction, statements



struct Ident /* 16byte */ {
	std::string *name=nullptr; // current name
	Ident *scp=nullptr;        // scope

	inline Ident(std::string *name, Ident *scp): name(name), scp(scp) {}
	inline Ident(Ident *scp=nullptr): scp(scp) {}

	inline auto operator<=>(Ident& other) const
		{ return std::tie(this->scp, this->name)<=>std::tie(other.scp, other.name); }
	inline auto operator<=>(const Ident& other) const
		{ return std::tie(this->scp, this->name)<=>std::tie(other.scp, other.name); }

	inline std::string txt(std::string sp="::") const {
		std::string ret;
		if (name) ret= *name;
		for (auto i=this->scp; i; i=i->scp) ret=std::format("{}{}{}", std::string(*i->name), sp, ret);
		return ret;
	}
};

template<typename _t> using Map   =std::unordered_map<Ident*, _t>;
template<typename _t> using Def   =std::pair<Ident* const, _t>;
template<typename _t> using Initls=std::initializer_list<_t>;
template<typename _t> using vec_t=std::vector<_t>;
using Code=std::vector<Exp*>;

enum class Visit :u8 /* visiblty state */ { PUB=0,   PRV=1,   PRT=2,  LOCKED=3 }; // need 2bits
enum class Status:u8 /* define   state */ { DEF=0,   UNDEF=1, OVLD=2, OVRD=3   }; // need 2bits
enum class TKind :u8 /* type      kind */ { TYP=0,   ENM=1,   UNI=2,  FN=3     }; // need 2bits
enum class Consty:u8 /* consty   state */ { NON=0,   RPL=1,   CTO=2            }; // need 2bits

constexpr std::string_view Visit_ls[]=
 	{ "pub", "prv", "prt", "locked" };
constexpr std::string_view Status_ls[]=
 	{ "def", "undef", "ovld", "ovrd" };
constexpr std::string_view Consty_ls[]=
	{ "", "rpl", "cto" };
constexpr std::string_view TKind_ls[]=
	{ "typ", "enm", "uni", "lmbd" };



inline bool is_expend  (TkT id) // is expretion ending
	{ return id==TkT::NEW_LINE || id==TkT::SIMICOLUMN || id==TkT::_EOF; }
inline bool is_initop  (TkT id) // is an init operation
	{ return id==TkT::ASSEGN || id==TkT::FRM || id==TkT::POINTS_EQ; }
inline bool is_def     (TkT id) // identefire
	{ return id==TkT::IDENT || id==TkT::PORT; }
inline bool is_op      (TkT id) // operation
	{ return istk_op(id) || istk_opkw(id); }
inline bool is_expblock(Tk  tk) // is block can use as expretion
	{ return tk.id == TkT::LB || tk.id == TkT::LPR || tk.id == TkT::TKWOWT; }
inline bool is_tydef   (TkT id) // is type definer & * && ** :: ident
	{ return id==TkT::IDENT || id==TkT::PORT || id==TkT::MUL || id==TkT::POW; }

// inline bool is_expblock(TkT id)
// 	{ return id == TkT::LB || id == TkT::LPR || id == TkT::TKWOWT; }



struct Flags /* 8byte, sheared flags struct for Typ, Var, Exp, Fn */ {
	union {
		u64 flags=0;
		struct {
			u32 loc; // All
			union /*2byte*/ {
				struct {
					bool isblk:1; // Exp
					u16  len:15;  // Exp: can hold expretion with len 32768 (32KB)
				};
                bool is_gen:1; // Typ
				// 16bit left for Var, Fn
			};
			union /*1byte*/ {
				TkT id; // Exp
				u8  n;  // Exp
                u8 lid; // Typ, Var, Fn
			};
			union /*2bit*/  {
				Consty consty:2; // Var, Fn, Exp
				TKind tk:2; // Typ
			};
			union /*2bit*/ {
				Status status:2; // Var, Fn, Typ
				bool isdeleted:1; // Exp
				// 1bit left for Exp
			};
			union /*2bit*/ {
				struct { bool sign:1, isptr:1; };  // Typ
				Visit visit :2;   // Var, Fn
				// 2bit left for Exp
			};
			union /*2bit*/ {
				struct {bool isflt:1, istxt:1;}; // Typ
				struct {
					union {
						bool glb:1;    // Var, Fn
						bool mcro_exp:1; // Exp (owned by a macro)
					};
					union {
						bool unsafe:1; // Exp, Fn
						bool mut:1; // Var
					};
				};
			};
		};
	};
};



struct TypHolder /* 16byte */ {
	/*
	 this is a wrapper for Typ,
	 so abstracted types that got defines
	 ain't doublicated for each
	 pointer or referance or array
	 proprieties
	 */
	Def<Typ> *row=nullptr; // the row type
	u32 ac=0; // up to 4GB array
	i8 pc=0, apc=0;
	TkT islit=TkT::UNK;
	bool mut:1=0, owned:1=0, opt:1=0, con:1=0, stream:1=0,
		 lmbd:1=0, arr:1=0;
	// 2bit left

	inline operator bool() { return row; }

	std::string txt();
};



struct Exp /* 40byte */ : Flags {
	using Flags::consty;
	using Flags::unsafe;
	using Flags::isblk;
	using Flags::loc;
	using Flags::id;
	using Flags::n;

	TypHolder typ={0};
	union {
		struct { Exp *opl=nullptr,  *opr=nullptr; }; // left & right operand
		struct { Exp *cond;         Code *code;   }; // if/ef struct
		struct { Exp* ops[2]; }; // operands
		Tk::Wrap  data; // literal data
		Def<Var> *var;  // use or decl of var
        Ident    *ident;
		Code     *blk;  // el/block of code
		// Def<Fn>  *lbl;
		struct {
			Def<Fn>  *fn;   // function name
			Exp      *args; // args as exp
		};
	};

	inline Exp(TkT id, u32 loc, Exp *opl, Exp *opr) :Flags() {
		this->loc=loc; this->id=id;
		this->opl=opl; this->opr=opr;
	}
	inline Exp(Tk tk) :Flags() {
		this->loc=tk.loc; this->id=tk.id;
		this->len=tk.len; this->data=tk.data;
	}
	inline Exp(TkT id, u32 loc) :Flags() { this->loc=loc; this->id=id; }
	inline Exp() :Flags() {}

	inline void* operator new(long unsigned int size) { return ::operator new(size); }
	static inline void operator delete(void *self) {
		if (self==nullptr) return;
		auto me=static_cast<Exp*>(self);
		if (me->isdeleted || me->mcro_exp) return;
		me->isdeleted=true;
		if (is_op(me->id)) {
			Exp::operator delete(me->opl);
			Exp::operator delete(me->opr);
		}
		std::free(me);
	}

	u32 arglen() /* lenght of args */ {
		if (id==TkT::COMMA && !this->isblk) {
			u32 i=0;
			if (opl) i+=opl->arglen();
			if (opr) i+=opr->arglen();
			return i;
		}
		return 1;
	}


	inline void findpos() /* calculating exp lenght */ {
		if (is_op(id)) {
			if (opl) {
				opl->findpos();
				len+=opl->len;
			}
			if (opr) {
				opr->findpos();
				len+=opr->len;
			}
		}
		if (len==0) len=1;
	}


	std::vector<Ident*> getypnames(); // get types names of args
	bool rpl();

	#ifdef DBG // in debug compile mode
		inline void print();
	#endif
};



struct Typ /* 48byte */ : Flags {
	Map<Var> vars;
	usz size=0;

	inline Typ()=default;

	#ifdef DBG // in debug compile mode
	inline void print(const Ident *name) { std::print("[[{}] {}]", size, txt(name)); }
		inline std::string txt(const Ident *name)
			{ return std::format("{} {}", TKind_ls[u8(this->tk)], name->txt()); }
	#endif


	inline bool fit_for(usz sz) { return sz*8 <= size; } // fit for value with sz
	inline bool fit_for(gmp::ibig *i) /* fit for int value */ {
		usz req= mpz_sizeinbase(i->get_mpz_t(), 2);
		return this->tk==TKind::ENM && req <= size;
	}
	inline bool fit_for(gmp::fbig *f) /* fit for float value */ {
		if (size<16) return false;
		return this->tk==TKind::ENM && this->isflt && f->get_prec()+7<= size;
	}
	inline bool fit_for(std::string *txt) /* fit for storing text */  {
		if (size<16*sizeof(usz)) return false;
		return this->istxt;
	}

    inline void alignment() {
        size/=8;
        size=std::bit_ceil(size)*8;
    }
};



struct Var /* 32byte */ : Flags {
	TypHolder typ={0};
	union {
		Exp *val=nullptr;
		Tks *tks;
	};

	inline Var()=default;

	#ifdef DBG // in debug compile mode
	inline std::string txt(const Ident *name) {
		return std::format("let {} {}", name->txt(), typ.txt());
	}
	inline void print(const Ident *name) {
		std::print("[{}", this->txt(name));
		if (this->consty==Consty::RPL && tks) {
			for (auto tk:*this->tks) tk.print();
			std::putchar('\n');
		}
		else if (val) {
			std::print("=");
			val->print();
		}
		std::putchar(']');
	}
	#endif
};



struct Fn /* 88byte */ : Flags {
	TypHolder typ={0};
	Map<Var> args, vars;
	union {
		Code *code;
		Tks *tks;
	};

	inline Fn()=default;

	#ifdef DBG // in debug compile mode
		inline std::string txt(const Ident *name) { return std::format("fn {} {}", name->txt(), typ.txt()); }
		inline void print(const Ident *name) {
			std::print("[{}(", this->txt(name));
			for (auto arg:args) {
				arg.second.print(arg.first);
				std::print(", ");
			}
			std::print(")]");
		}
	#endif

	inline std::vector<Ident*> getypnames() {
		std::vector<Ident*> typnames;
		for (auto [name, var]:args)
			typnames.push_back(var.typ.row->first);
		return typnames;
	}
};



struct Pkg /* 104byte */ {
	Ident *name;
	Map<Typ> typs;
	Map<Var> vars;
	Map<Fn>  fns;

	Pkg(Ident *name=nullptr) :name(name) {}

	int save /* save to   .ucc/`name`.bkj */ ();
	int load /* load from .ucc/`name`.bkj */ ();
	#ifdef DBG // in debug compile mode
		inline void print() {
			if (name) std::println("pkg {}", name->txt());
			for (auto typ:typs) {
				typ.second.print(typ.first);
				std::putchar('\n');
			}
			for (auto var:vars){
				var.second.print(var.first);
				std::putchar('\n');
			}
			for (auto fn:fns) {
				fn.second.print(fn.first);
				std::putchar('\n');
			}
		}
	#endif
};




#ifdef DBG // in debug compile mode
	inline void Exp::print() {
		switch (id) {
			case TkT::TXT_LIT:   std::print("{}", *data.txt); break;
			case TkT::RG_LIT:    std::print("${}x{}", data.rgid, data.rgsz); break;
			case TkT::CHAR_LIT:  std::print("{}", int(data.c)); break;
			// case TkT::BOOL_LIT:  std::print("{}", data.b); break;
			case TkT::NUM_LIT:   std::print("{}", data.i->get_str(10)); break;
			case TkT::FLOAT_LIT: {
				gmp::exp exp;
				std::print("{}", data.f->get_str(exp));
			}	break;

			case TkT::LET:
				var->second.print(var->first);
				break;
			case TkT::VARCALL:
				std::print("{}", var->first->txt());
				break;
			case TkT::FNCALL:
				// fn->second.print(fn->first);
				std::print("{}", fn->first->txt());
				break;
			default:
				if (is_op(id)) {
					std::putchar(' ');
					#define DEFAULT_PRINT \
						if (opl) opl->print();\
						std::print("{}", tkls[u8(id)]);\
						if (opr) opr->print()
					if (typ) {
						std::print("({})(", typ.txt());
						DEFAULT_PRINT;
						std::putchar(')');
					}
					else {
						DEFAULT_PRINT;
					}
					std::putchar(' ');
					return;
				}
		}
	}
#endif



std::vector<Ident*> Exp::getypnames() {
	std::vector<Ident*> typnames;
	if (id==TkT::COMMA && !this->isblk) {
		if (opl) {
			auto typns=opl->getypnames();
			typnames.resize(typnames.size() + typns.size());
			for (auto it:typns) typnames.push_back(it);
		}
		if (opr) if (opr->typ)
			typnames.push_back(opr->typ.row->first);
	}
	else if (typ) typnames.push_back(typ.row->first);
	return typnames;
}



bool Exp::rpl() {
	if (istk_lit(id)) return true;
	if (is_op(id)) {
		bool ret=true;
		if (opl) ret= ret && opl->rpl();
		if (opr) ret= ret && opr->rpl();
		return ret;
	}
	switch (id) {
		case TkT::LET:
		case TkT::VARCALL:
			if (!var) return false;
			return var->second.consty==Consty::RPL;
		case TkT::FN:
		case TkT::FNCALL:
			if (!fn) return false;
			return fn->second.consty==Consty::RPL;
		default:break;
	}
	return false;
}



std::string TypHolder::txt() {
	if (!row) return "";
	return std::format("{}{}{}{}{}{}[{}]{}{}{}{}{}{}",
			mut? "^":"",
			owned? "!":"",
			opt? "?":"",
			con? "#":"",
			stream? "...":"",
			std::string(apc, (apc<0)?'&':'*'), ac+1,
			std::string(pc, (pc<0)?'&':'*'),
			row->second.sign? "-":"",
			row->second.isptr? "*":"",
			row->second.isflt? ".":"",
			row->second.istxt? ":":"",
			row->first->txt());
}
