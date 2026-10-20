inline void expect(std::string msg, usz loc, usz len) { LEX.perr(std::format("expect {}", msg), loc, len); }
inline void redef(std::string msg, usz loc, usz len) { LEX.perr(std::format("redefine {}", msg), loc, len); }
inline void undef(std::string msg, usz loc, usz len) { LEX.perr(std::format("undef {}", msg), loc, len); }
inline void unexpected(usz loc, usz len) { LEX.perr("unexpected token", loc, len); }



inline void push_built_in() {
    push_file("../configs/builtin_typ.cyv");
    push_file("../configs/consts.cyv");
    push_file("../configs/arch64.cyv");
    push_file("../configs/typ.cyv");
}



inline bool new_lex(std::string file="") {
	lexs.push_back(Lex());
	lexidx=lexs.size()-1;
	LEX.symbols=&symbols;
	LEX.integers=&integers;
	LEX.floats=&floats;
	LEX.nowrg=nowrg;
	LEX.tbs=tbs;
	if (file.empty()) return true;
	return lexs[lexidx].init(file);
}

inline bool new_lex(std::string *text) {
	lexs.push_back(Lex());
	lexidx=lexs.size()-1;
	LEX.symbols=&symbols;
	LEX.integers=&integers;
	LEX.floats=&floats;
	LEX.nowrg=nowrg;
	LEX.tbs=tbs;
	if (!text) return true;
	return lexs[lexidx].init(text);
}



// template<typename ...Args>
inline TksView liner_block(TksView tks, TkT end=TkT::NEW_LINE) /* get TksView for a chunk as sub tokens list */ {
	usz i=idx;
	for (; i<tks.size(); i++)
		if (tks[i].id==end)
			break;
	return TksView(tks.begin()+idx, tks.begin()+i);
}



template<typename Fn, typename... Args>
inline auto handleblk(TksView tks, Fn &&fn, Args &&...args) {
	using RetType = std::invoke_result_t<Fn, Parser*, TksView, Args...>;
	usz i=idx; idx=0;
	if constexpr (!std::is_same_v<RetType, void>) {
		auto ret=std::invoke(std::forward<Fn>(fn), this, *tks[i].data.block, std::forward<Args>(args)...);
		idx=i;
		return (RetType)ret;
	}
	else std::invoke(std::forward<Fn>(fn), this, *tks[i].data.block, std::forward<Args>(args)...);
	idx=i;
}

template<typename Fn, typename... Args>
inline auto handle_codeblk(TksView tks, Fn &&fn, Args &&...args) {
	using RetType = std::invoke_result_t<Fn, Parser*, TksView, Args...>;
    if (istk_blk(tks[idx].id)) {
		usz i=idx; idx=0;
		if constexpr (!std::is_same_v<RetType, void>) {
			auto ret=std::invoke(std::forward<Fn>(fn), this, *tks[i].data.block, std::forward<Args>(args)...);
			idx=i+1;
			return (RetType)ret;
		}
		else std::invoke(std::forward<Fn>(fn), this, *tks[i].data.block, std::forward<Args>(args)...);
		idx=i+1;
	}
    else {
        if (tks[idx].id==TkT::DO) idx++;
		TksView _tks=liner_block(tks);
		usz i=idx; idx=0;
		if constexpr (!std::is_same_v<RetType, void>) {
			auto ret=std::invoke(std::forward<Fn>(fn), this, _tks, std::forward<Args>(args)...);
			idx=i+1+_tks.size();
			return (RetType)ret;
		}
		else std::invoke(std::forward<Fn>(fn), this, _tks, std::forward<Args>(args)...);
		idx=i+_tks.size()+1;
    }
	quit:
	if constexpr (!std::is_same_v<RetType, void>) return (RetType)nullptr;
	else return;
	// ASK for block
	goto quit;
}






inline gmp::ibig *push_num(gmp::ibig i) { return const_cast<gmp::ibig*>(&*integers.insert(i).first); }
inline gmp::fbig *push_num(gmp::fbig f) { return const_cast<gmp::fbig*>(&*floats.insert(f).first); }
inline std::string *push_symbol(std::string txt) {
	if (txt.empty()) return nullptr;
	return const_cast<std::string*>(&*symbols.insert(txt).first);
}

inline Ident *push_ident(std::string name, Ident *scp) { return push_ident(Ident(push_symbol(name), scp)); }
inline Ident *push_ident(std::string *name, Ident *scp) { return push_ident(Ident(name, scp)); }
inline Ident *push_ident(Ident ident) /* to keep real memory in `defs` */ {
	if (!ident.name) return nullptr;
	return const_cast<Ident*>(&*defs.insert(ident).first);
}
inline Ident *peek_ident(TksView tks, Ident *tscope=nullptr) {
	usz loc=tks[tks.size()-1].loc, len=tks[tks.size()-1].len;
	Ident ret(tscope);
	redo:
	switch (tks[idx].id) {
		case TkT::IDENT:
			if (ret.name) goto done;
			ret.name=tks[idx].data.txt;
			idx++;
			goto redo;
		case TkT::PORT:
			idx++;
			return peek_ident(tks, push_ident(ret));
		default: break;
	}
	done:
	if (!ret.name) {
		loc=tks[idx].loc;
		len=tks[idx].len;
		if (!tscope)
			LEX.perr("filed to parse identifire", loc, len);
		return tscope;
	}
	return push_ident(ret);
}



inline TypHolder peek_typ(TksView tks) {
	TypHolder ret{};
	i8 *pc=&ret.pc;
	peek_pc:
	while (idx<tks.size()) {
		switch (tks[idx].id) {
			// flags
			case TkT::DDOT: ret.stream=true;  break;
			case TkT::NOT:  ret.owned=true;   break;
			case TkT::QMRK: ret.opt=true;     break;
			case TkT::HASH: ret.con=true;     break;
			case TkT::XOR:  ret.mut=true;     break;

			// pointer counter
			case TkT::AND_SIGN:(*pc)--; break;
			case TkT::POW:	   (*pc)+=2;break;
			case TkT::AND:	   (*pc)-=2;break;
			case TkT::MUL:	   (*pc)++; break;
			default: goto quit_loop;
		}
		idx++;
	} quit_loop:

	if (!ret.arr && tks[idx].id==TkT::LPR) {
		ret.apc=ret.pc; ret.pc=0;
		auto szexp=handleblk(tks, &Parser::parse_exp, TkT::SIMICOLUMN);
		if (!szexp) goto is_nonsized_arr;
		if (szexp->consty!=Consty::RPL) {
			expect("const len", tks[idx-1].loc, tks[idx-1].len);
			return ret;
		}
		if (szexp->id!=TkT::NUM_LIT) {
			expect("unsigned number as array len", tks[idx-1].loc, tks[idx-1].len);
			return ret;
		}
		ret.ac=(u32)szexp->data.i->get_ui();
		is_nonsized_arr: ret.arr=true;
		goto peek_pc;
	}

	switch (tks[idx].id) {
		case TkT::IDENT:
		case TkT::PORT: {
			usz loc=tks[idx].loc;
			auto typname=peek_ident(tks, scope);

			if (!typname) goto retnull;
			if (auto it=getyp(typname)) {
				ret.row=&*it;
				goto retnull;
			}

			auto [it, add]=pkg.typs.insert({typname, Typ{}});
			if (add) {
				it->second.status=Status::UNDEF;
				it->second.loc=loc;
			}
			ret.row=&*it;
		} break;
		// case TkT::ENM: {
		// 	if (tks[idx].id==TkT::LPR) {
		// 	}
		// } break;
		default:
            break;
			unexpected(tks[idx].loc, tks[idx].len);
	}
	retnull:
		return ret;
}



inline Def<Fn> *getfn(Ident *ident, std::vector<Ident*> typnames) {
	if (!ident) return nullptr;
	Ident *mangled=mangle(ident, typnames);
	if (auto ok=pkg.fns.find(mangled); ok!=pkg.fns.end())
		return &*ok;
	return nullptr;
}
inline Def<Typ> *getyp(Ident *ident) {
	if (!ident) return nullptr;
	if (auto it=pkg.typs.find(ident); it!=pkg.typs.end())
		return &*it;
	return nullptr;
}

inline Def<Var> *getvar(Ident *ident) {
	if (!ident) return nullptr;
	if(cvs != &pkg.vars)
		if (auto ok=cvs->find(ident); ok!=cvs->end())
			return &*ok;
	for (auto other:ovs)
		if (auto ok=other->find(ident); ok!=other->end() && other!=&pkg.vars)
			return &*ok;
	if (auto ok=pkg.vars.find(ident); ok!=pkg.vars.end())
		return &*ok;
	return nullptr;
}



inline Tks askfor(std::string msg, bool lexing=false) {
	if (!ask) return Tks{};
	std::print("{}? [Y/n]: ", msg); {
		char c;
		std::cin >> c;
		if (c=='n' || c=='N') return Tks{};
	}
	if (lexing) {
		std::print("--> ");
		std::string txt;
		std::getline(std::cin, txt);
		Lex blex;
		blex.init(&txt);
		return blex.tks();
	}
	else return {Tk(TkT::_EOF, 0, 0)};
}



inline Def<Fn> *push_fn(Ident *name, Fn fn) {
	auto [it, ok]=pkg.fns.insert({name, fn});
	Def<Fn> *fn_=&*it;
	if (!ok) {
		if (fn_->second.status!=Status::UNDEF && fn_->second.status!=Status::OVLD) {
			redef(std::format("fn `{}`", name->txt()), fn.loc, 2);
			return nullptr;
		}
		fn_->second=fn;
		fn_->second.visit=g_visit;
	}
	return fn_;
}



inline Def<Var> *push_var(Ident *name, Var var) {
	auto old=cvs;
	if (glb) cvs=&pkg.vars;
	auto [it, ok]=cvs->insert({name, var});
	Def<Var> *var_=&*it;
	if (!ok) {
		redef(std::format("var `{}`", name->txt()), var.loc, 3);
		cvs=old;
		return nullptr;
	}
	cvs=old;
	var_->second.visit=g_visit;
	return &*it;
}



inline Def<Typ> *push_typ(Ident *name, Typ typ) {
	auto [it, ok]=pkg.typs.insert({name, typ});
	Def<Typ> *typ_=&*it;
    if (typ_->second.is_gen) {
        typ_->second=typ;
        goto quit;
    }
	if (!ok) {
		if (typ_->second.status!=Status::UNDEF && typ_->second.status!=Status::OVLD) {
			redef(std::format("typ `{}`", name->txt()), typ.loc, 2);
			return nullptr;
		}
		typ_->second=typ;
	}
    quit: return typ_;
}