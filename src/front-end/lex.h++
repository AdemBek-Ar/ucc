/* ========================================================================== *
||   Lexer file: hold the Lexer struct (Lex) and relatives                   ||
||        *OpT        : operation char type                                  ||
||        *OP_TRAITS  : operations lookup table helper                       ||
||        *num_lookup : numbers lookup table, for check the digit base       ||
||        *getk_id    : liner search the token id                            ||
||        *get_bs_char: getting backslash token                              ||
||        *Reader     : Reader struct that read from file or string          ||
||        *Lex        : main Lex struct lexing text to tokens                ||
 * ========================================================================== */

enum class OpT : u8 /* the kind of operation char, 1byte*/ {
	OP_NONE   = 0,
	OP_VALID  = 1 << 0, // char is operation
	OP_DOUBLE = 1 << 1, // char is doublecatable operation (like ++)
	OP_EQUAL  = 1 << 2  // char is equilable     operation (like +=)
};



static constexpr auto OP_TRAITS = []() /* compile time operation hash table */ {
	std::array<u8, 256> map{};
	for (int i = 0; i < 256; ++i)
		if ((i>'9' && i<'A')||(i>'z' && i<='~')||(i>32  && i<'0')||(i>'Z' && i<'_'))
			map[i] |= u8(OpT::OP_VALID);
	const char* doubles = "+-*/&|<>:.~=#";
	for (int i = 0; doubles[i]; ++i) map[doubles[i]] |= u8(OpT::OP_DOUBLE);
	const char* equals = "=|-+*$@<>?^&!~%:";
	for (int i = 0; equals[i]; ++i) map[equals[i]] |= u8(OpT::OP_EQUAL);

	return map;
}();



static constexpr auto num_lookup = []() /* to queicly get the number base from the char */ {
	std::array<u8, 256> map{};
	for (int i=0; i < 256; ++i) map[i] = 255;
	for (char c='0'; c<='9'; c++) map[static_cast<u8>(c)] = c -'0';
	for (char c='A'; c<='Z'; c++) map[static_cast<u8>(c)] = c -'A'+10;
	for (char c='a'; c<='z'; c++) map[static_cast<u8>(c)] = c -'a'+10;
	return map;
}();



inline cyv::TkT getk_id(std::string_view txt, cyv::TkT s, cyv::TkT e, cyv::TkT non) {
	for (u8 n = u8(s); n < u8(e); n++)
		if (txt == cyv::tkls[n])
			return static_cast<cyv::TkT>(n);
	return non;
}

inline ch32 get_bs_char(ch32 ch) /* backslash chars */ {
	switch (ch) {
		case '\\': return '\\';
		case 'n': return '\n';
		case 't': return '\t';
		case 'r': return '\r';
		case 'b': return '\b';
		case 'e': return '\e';
		case 'v': return '\v';
	}
	return ch;
}



struct Reader /* 48byte */ {
	std::string path;     // file path or stream name
	union {
		std::string *tsrc; // text source
		FILE *fsrc;        // file source
	};
	u32 idx=0;
	bool isfile:1=false;
    bool isdeleted:1=false;

	inline Reader()=default;

	int init(sfs::path p) /* init from file */ {
		path=p.string();
		fsrc=std::fopen(path.c_str(), "r");
		if (!fsrc) {
			::perr("can't open file \"{}\"", path);
			return 1;
		}
		isfile=true;
		return 0;
	}
	int init(std::string *tsrc) /* init from text */ {
		if (!tsrc) {
			::perr("can't get text \"{}\"", path);
			return 1;
		}
		path="{text stream}";
		this->tsrc=tsrc;
		isfile=false;
		return 0;
	}

	inline void operator delete(void* self) {
        auto me=static_cast<Reader*>(self);
        if (me->isdeleted) return;
		if (me->isfile) {
            me->isdeleted=true;
			// closing file
			if (me->fsrc) {
				std::fclose(me->fsrc);
				me->fsrc=nullptr;
			}
		}
		else me->tsrc=nullptr; // do nothing with string
	}



	inline u32 set(usz pos) /* move reading courser */ {
		if (isfile) return std::fseek(fsrc, idx=(u32)pos, 0);
		return idx=(u32)pos;
	}
	inline void pre(ch32 c) /* reading and move courser backward */ {
		if (isfile) std::ungetc(c, fsrc);
		idx--;
	}
	inline ch32 next() /* reading and moving forward */  {
		idx++;
		if (isfile) return std::fgetc(fsrc);
		if (idx > tsrc->size()) return EOF;
		return (*tsrc)[idx];
	}
	inline ch32 peek() /* get current char */ {
		if (isfile) {
			ch32 c=std::fgetc(fsrc);
			std::ungetc(c, fsrc);
			return c;
		}
		if (idx > tsrc->size()) return EOF;
		return (*tsrc)[idx];
	}
};



struct Lex /* 88byte */: Reader {
	std::set<std::string> *symbols; // symbols table in parser
	std::set<gmp::ibig> *integers; // integers table in parser
	std::set<gmp::fbig> *floats; // floats table in parser
	std::vector<usz> lindexs={0}; // line indexer

	u8 tbs=4; // tabs by spaces (default is 4)
	u8	usm:2=0,   // lexing mode (normal, assmbly keyword, assmbly line, assmbly block)
	    nowrg:1=0, // no warning
		ec:1 =0;   // error code
	// 13bit left

	inline Lex()=default;
	using Reader::path;
	using Reader::peek;
	using Reader::next;
	using Reader::set;
	using Reader::pre;
    
    
	inline void operator delete(void* self) { static_cast<Lex*>(self)->Reader::operator delete(self); }

	// the main get token function
	std::expected<Tk,Err> getk(Tks &ret, u16 tabs) {
		ch32 c=next(); // current
		u32 loc=idx;   // location
		switch((char)c) {
			case '\n': return Tk(TkT::NEW_LINE, loc); // return new line token
			case '\'': // char or text literal
			case '"' : /* text literal */ {
				ch32 e=c;                   // text end char
				std::string buf;            // text bufer
				Tk txt(TkT::TXT_LIT, loc);
				while (i32(c=next())!=EOF) /* peekup "text" */ {
					if(c=='\\') /* skip \" or \' (as `e` value) */ {
						c=next();
						buf+=get_bs_char(c);
						continue;
					}
					if(c==e) break;
					buf+=c;
				}
				txt.len=idx-loc;

				// is it char lietal
				if (e=='\'' && !buf.empty()) {
					if(u8 clen=utf8_len(buf[0]); buf.size()==clen) {
						txt.id=TkT::CHAR_LIT;
						txt.data.c= buf[0];
						goto ret_txtk;
					}
				}

				/* is it string literal */ {
					buf.shrink_to_fit();		  // less memory useage
					auto ok=symbols->insert(buf); // push in symbols table
					txt.data.txt=const_cast<std::string*>(&(*ok.first)); // get text adders
				}
				ret_txtk: return txt;
			}
			// comments :) handling
			case '/': {
				ch32 ch=peek();
				switch (ch) {
					case '/': // skip lined comments
						while (i32(c=next())!=EOF)
							if (c=='\n') {
								pre(c);
								break;
							}
						return Tk();
					case '*': /* skip blocked comments */
						while (i32(c=next())!=EOF)
							if (c=='*' && peek()=='/') {
								next();
								break;
							}
						return Tk();
					default: break;
				}
				goto peek_up;
			}
			// backslash incoding
			case '\\':{
				ch32 ch=peek();
				switch(ch) {
					default:break;
					case '\n': // skip this new line in token processing
						lindexs.push_back(idx);
						goto sarntk;

					case 'W': // active warning
						nowrg=1;
						goto sarntk;
					case 'M': // disactive warning
						nowrg=0;
						goto sarntk;
// 					case 'A': // active assebmly
// 						usm=3;
// 						goto sarntk;
					case 'a': // active one assebmly keyword
						usm=1;
						goto sarntk;
					case 'A': // active one assebmly line
						usm=2;
						sarntk:
							next();
							return Tk();

					case 'I': /* include */ {
						next();
						auto ok=getk(ret,tabs);
						if (ok.has_value()) {
							Tk ipatk=ok.value();
							if (ipatk.id!=TkT::TXT_LIT)
								return std::unexpected{Err{"expect a text literal for include path", loc, idx-loc}};
							if (!sfs::exists(*ipatk.data.txt))
								return std::unexpected{Err{"include path; such file or directory", loc, idx-loc}};
							ipatk.id=TkT::INCLUDE;
							return ipatk;
						}
						else perr(ok.error()); // handle errors
					}

					case 'n': // deal with \\n as new line
						next();
						return Tk(TkT::NEW_LINE, loc);

					case '^': /* move reader pointer to position in pre-line and get token from */ {
						usz len=getln(idx);
						if (len==0) /* there is no pre-line */ {
							next();
							return std::unexpected{Err{"no previos line", loc, 2}};
						}
						set(lindexs[len-1]+(loc-lindexs[len])-1);
						auto tk=getk(ret,tabs);
						/* we need this to ignore the error in infinit loop case:
							somecode \^ ... morecode
							mehrcode\^
						*/
						set(loc+2);
						return tk;
					}
					/* TODO:
					      case '\\': to get token with the same index in pre-line according to \n token in `ret`*
					 */
				}
			}
			{ // peekup blocks
				ch32 e; // end block char
				TkT bid;// block id
				case '`': e='`'; bid=TkT::TKWOWT; goto peekup_block;
				case '[': e=']'; bid=TkT::LPR; goto peekup_block;
				case '{': e='}'; bid=TkT::LBR; goto peekup_block;
				case '(': e=')'; bid=TkT::LB;
				peekup_block:
				// init block token
				Tk b=Tk(bid, loc, 0, { .block=new Tks });
				while(i32(c=next())!=EOF) {
					while (std::isspace((char)c)) c=next();
					pre(c);
					if (c==e) {
						next();
						break;
					}

					auto ok=getk(ret,tabs+1); //peekup token in block
					if (ok.has_value()) push(*b.data.block, ok.value(), tabs+1);
					else perr(ok.error()); // handle errors
				}
				b.len=idx-loc;
				/*if (b.data.block->empty()) {
					delete b.data.block;
					b.data.block=nullptr;
				}
				else */
				b.data.block->shrink_to_fit();
				return b;
			}

			case '$': { // ragister literal
				c=next();
				if (!std::isdigit(c)) {
					perr("expect a number for register id", loc, idx-loc);
					return Tk(TkT::UNK, loc, idx-loc);
				}
				// getting register id  $0, $1, ...
				std::string txt; txt+=c;
				while (i32(c=next())!=EOF && std::isdigit(c)) txt+=c;
				u16 rgst_id=std::stoul(txt), rgst_sz=8;
				if (c=='x') /* getting register size $0x64=rax, $1x8=bl */ {
					c=peek();
					if (!std::isdigit(c)) {
						pwrg("expect a number for register size", loc, idx-loc);
						goto push_register_info;
					}
					txt.clear();
					while (i32(c=next())!=EOF && std::isdigit(c)) txt+=c;
					rgst_sz=std::stoul(txt);
				}
				push_register_info:
				return Tk(TkT::RG_LIT, loc, idx-loc, Tk::Wrap{.rgid=rgst_id, .rgsz=rgst_sz});
			}
		}

		peek_up:
		std::string buf;
		buf+=c;
		// peekup identifier
		if (std::isalpha((char)c) || c=='_') {
			while(i32(c=next())!=EOF) {
				if(std::isalnum((char)c) || c=='_') {
					buf+=c;
					continue;
				}
				break;
			}
			pre(c);

			if (buf.size() < 4) /* searching for keyword */ {
				if (usm) /* assembly keyword */ {
					if (usm==1) usm=0;
					if(TkT id=getk_id(buf, tkf_usmkw, tkl_usmkw, TkT::IDENT); id!=TkT::IDENT)
						return Tk(id, loc, idx+1-loc);
				}
				else /* normal keyword */ if(TkT id=getk_id(buf, tkf_kw, tkl_kw, TkT::IDENT); id!=TkT::IDENT)
					return Tk(id, loc, idx+1-loc);
			}

			buf.shrink_to_fit();
			auto [it, ok]=symbols->insert(buf);
			return Tk(TkT::IDENT, loc, idx+1-loc, {.txt=const_cast<std::string*>(&*it)});
		}

		// peekup numbers
		if (std::isdigit((char)c)) {
			u8 base=10;
			TkT id=TkT::NUM_LIT;

			if(c=='0') /* select the base */ {
				buf.clear();
				switch(c=next()) {
					default :
                        pre(c);
                        break;
					case 'h': base=16; break; // hexadesimal
					case 'o': base=8;  break; // octal
					case 'b': base=2;  break; // binary
					case 'x': /* custom base */ {
						std::string bbuf;
						c=next();
						// peekup base test
						while(i32(c=next())!=EOF && std::isdigit((char)c))
							bbuf+=c; /* you can break base selection with any char
									    even `0x23"23` or `0x9;11` will work      */
						next();
						if (!bbuf.empty()) {
							base=static_cast<u8>(std::stoul(bbuf));
							if (base>36) /* ASCII liters limit */ base=36;
							if (base==0) base=1;
						}
						break;
					}
				}
			}

			// peekup the number value
			while(i32(c=next())!=EOF) {
				if(num_lookup[c] < base) buf+=c;
				else if (c=='_') continue; // 1_000_000 allowed
				else if (c=='.') {
					if (base!=10) {
						pwrg("the non desimal values can't be a float", idx, 1);
						continue;
					}
					if(id==TkT::NUM_LIT) {
						id=TkT::FLOAT_LIT;
						buf+=c;
					}
				}
				else break;
			}
			pre(c);
            if (buf.empty()) buf='0';
			switch(id) {
				case TkT::FLOAT_LIT:{
					auto [it, ok]=floats->insert(gmp::fbig(buf, gmp::bitcnt(50), (int)base));
					return Tk(id, loc, idx-loc, {.f=const_cast<gmp::fbig*>(&*it)});
				}
				default: /* can only be NUM_LIT */{
					auto [it, ok]=integers->insert(gmp::ibig(buf, (int)base));
					return Tk(id, loc, idx-loc, {.i=const_cast<gmp::ibig*>(&*it)});
				}
			}
		}

		// peekup operation
		if (c>OP_TRAITS.size()) goto unknown_char;
		u8 trait = OP_TRAITS[c];
		if (trait & u8(OpT::OP_VALID)) {
			u32 op_hash=u32(c);
			ch32 ch = peek();
			u32 len=1;
			if (ch == c && (trait & u8(OpT::OP_DOUBLE))) {
				op_hash |= (ch << 8);
				next();
				len++;
				ch = peek();
			}
			if (ch == '=' && (trait & u8(OpT::OP_EQUAL))) {
				op_hash |= (ch << (len * 8));
				next();
				len++;
			}
			else if (ch == '>' && (c == '-' || c == '=')) {
				op_hash |= (ch << (len * 8));
				next();
				len++;
			}

			TkT id=TkT::UNK;
			for (const auto& pair : op_lookup)
				if (pair.hash == op_hash) {
					id = pair.id;
					break;
				}
			if (id==TkT::UNK) return std::unexpected{Err{"unk operator", loc, idx-loc+1}};
			return Tk(id, loc, idx-loc);
		}

		unknown_char:
			if (std::isspace(c)) return Tk();
			return std::unexpected{Err{"unknown character", loc, 1}};
	}



	inline Tks tks(u16 tabs=0) {
		Tks ret; // the returned block
		for(ch32 c=peek(); i32(c)!=EOF; c=peek()) {
			while (c==' '||c=='\t'||c=='\r'||c=='\b') /* skip only this white spaces */ {
				next();
				c=peek();
			}
			if(i32(c)==EOF) break;

			auto ok=getk(ret,tabs);
			if (ok.has_value()) {
				if (push(ret, ok.value(), tabs)==1) /* push token */ {
					ret.shrink_to_fit();
					return ret;
				}
			}
			else perr(ok.error()); // handle error
		}
		ret.shrink_to_fit();
		return ret;
	}



	inline u8 push(Tks &ret, Tk tk, u16 tabs) {
		switch (tk.id) {
			default : ret.push_back(tk);
			case TkT::UNK : break; // skip
			case TkT::NEW_LINE: {
				lindexs.push_back(tk.loc);
				if (usm==2) usm=0; // disactivate assembly key words
				if (usz len=ret.size(); len && ret[len-1].id!=TkT::NEW_LINE) ret.push_back(tk);
				ch32 c=peek();
				while (i32(c=next())!=EOF)
					if (c!='\n') {
						pre(c);
						break;
					}
					else lindexs.push_back(idx);


				recounting:
				u16 count=0; // get block by tabs
				while(i32(c=next())!=EOF) {
					switch(c) {
						case ' ' : count++;  continue;
						case '\t': count+=tbs; continue;
						default: break;
					}
					break;
				}
				pre(c);
				if (i32(c)==EOF) return 1;

				count/=tbs;
				if (count<tabs) {
					set(lindexs[lindexs.size()-1]);
					return 1;
				}
				else if (count>tabs) {
					Tk b(TkT::TAB, tk.loc, tk.len);
					b.data.block=new Tks(tks(count));
                    ret.push_back(b);
					goto recounting;
				}
			}
		}
		return 0;
	}



	inline usz getln(usz loc) /* get line index */ {
		std::span<usz> slc(lindexs.begin(), lindexs.end());
		re_srearching:
		if (slc.size()==1) return &slc[0] - &lindexs[0];
		usz helf=slc.size()/2;
		usz v=slc[helf];
		if (loc==v) return helf+ &slc[0] - &lindexs[0];
		if (loc>v) {
			slc = std::span<usz>(slc.begin()+helf, slc.size()-helf);
			goto re_srearching;
		}
		if (loc<v) {
			slc = std::span<usz>(slc.begin(), slc.size()-helf);
			goto re_srearching;
		}
		return 0;
	}



	inline void perr(std::string_view msg, u32 loc, u32 len) {
		ec=1;
		::perr("{}", msg);
		ploc(loc, len);
	}
	inline void perr(Err err) {
		ec=1;
		::perr("{}", err.msg);
		ploc(err.loc, err.len);
	}
	inline void pwrg(Err err) {
		if (nowrg) return;
		::pwrg("{}", err.msg);
		ploc(err.loc, err.len, CLI::YELLOW);
	}
	inline void pwrg(std::string_view msg, u32 loc, u32 len) {
		if (nowrg) return;
		::pwrg("{}", msg);
		ploc(loc, len, CLI::YELLOW);
	}


	// print location
	void ploc(u32 loc, u32 len, const char* color=CLI::RED) {
		usz ln=getln(loc),
			lnpos=lindexs[ln],
			column=loc-lindexs[ln++];
		// if (ln<lindexs.size()) lnpos=lindexs[ln];
		if (loc>0) loc--;
		std::print("└──> {}:{}:{}\n {:>4} | ", path, ln, column, ln);
		usz pos=idx;
		set(lnpos);
		for (usz i=lnpos; true; i++) {
			ch32 c=next();
			if (c=='\0'||c=='\n'||i32(c)==EOF) break;
			if (i==loc) std::print("{}", color);
			if (i==loc+len) std::print("{}", CLI::NON);
			std::putchar(c);
		}
		std::print("\n      | ");

		set(lnpos);
		for (usz i=lnpos; true; i++) {
			ch32 c=next();
			if (c=='\0'||c=='\n'||i32(c)==EOF) break;
			if (i==loc) {
				std::print("{}^", color);
				while (i < loc+len-1) {
					next();
					std::putchar('~');
					i++;
				}
				std::print("{}", CLI::NON);
			}
			switch(c) {
				case '\v': case '\t': std::putchar(c); break;
				default: std::putchar(' ');
			}
		}
		std::putchar('\n'); std::putchar('\n');
		set(pos);
	}


	// print help is spiciale
	inline void phlp(std::string_view msg, u32 loc, u32 len,
					 std::function<std::string(std::string_view msg, u32 loc, u32 len)> help)
	{
		std::function<std::string(Err)> wrap=[&](Err err) { return help(err.msg, err.loc, err.len); };
		phlp (Err{msg, loc, len}, wrap);
	}
	void phlp(Err err, std::function<std::string(Err)> help) {
		usz ln=getln(err.loc),
			lnpos=lindexs[ln];
		u32 column=u32(err.loc-lindexs[ln++]);

		::phlp("{}", err.msg);
		std::print("└──> {}:{}:{}\n {:>4} | ", path, ln, column, ln);

		usz pos=idx;
		set(lnpos);
		std::string line; line.reserve(column);
		for (usz i=lnpos; true; i++) {
			ch32 c=next();
			if (c=='\0'||c=='\n'||i32(c)==EOF) break;
			line+=c;
		}

		std::println("{}\n	  | \n", help(Err{line, column, err.len}));
		set(pos);
	}
};