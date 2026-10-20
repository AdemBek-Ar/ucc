struct Parser /* 448byte, limit: 512byte */ {
    vec_t<std::string> files;      // file that we will parse
    std::set<std::string> symbols; // symbols table
    std::set<gmp::ibig> integers;  // integers table
    std::set<gmp::fbig> floats;    // floats table
    std::set<Ident> defs;          // identifires
    vec_t<Lex> lexs;

    Ident *scope=nullptr; // buffer
    Pkg pkg;   // main abstracted package
    vec_t<Map<Var>*> ovs; // other vars srcs
    Map<Var> *cvs; // current vars scope
    TypHolder defaultyp={};
    usz idx=0, lexidx=0;

    #define LEX lexs[lexidx]

    u8 tbs=4, PTRSIZE=8, BUILT_IN_TYP_COUNT=31;
    bool nowrg:1=0, ask:1=0, glb:1=0, cstyl:1=0, abt:1=1;
    Visit g_visit:2=Visit::PUB;

    inline Parser() {
        new_lex(); // init the first lexer
        cvs=&pkg.vars;
    }

    #include "parser/fileAPI.h++"
    #include "parser/inlined.h++"
    #include "parser/mangle.h++"

    #include "parser/macro.h++"
    #include "parser/exp.h++"
    #include "parser/typ.h++"
    #include "parser/let.h++"
    #include "parser/mod.h++"
    #include "parser/fn.h++"


    inline void parse(TksView tks) {
        for (; idx<tks.size() && tks[idx]; idx++) switch (tks[idx].id) {
            case TkT::PKG: parse_pkg(tks); continue; // i want to change how this work
            case TkT::MOD: parse_mod(tks); continue;
            case TkT::_EOF: return;
            case TkT::HASH:
                idx++;
                if (tks[idx].id==TkT::LET) goto parse_let_;
                parse_macro(tks); continue;
            case TkT::LET: parse_let_:
                idx++;
                parse_let(tks);
            default: continue;

            case TkT::UNI:
            case TkT::ENM:
            case TkT::TYP:
                idx++;
                if (tks[idx].id==TkT::EQ) {
                    idx++;
                    defaultyp=peek_typ(tks);
                    continue;
                }
                parse_typ(tks);
                continue;

            case TkT::FN:
                idx++;
                parse_fn(tks);
                continue;

            case TkT::IEP:
                idx++;
                parse_mainfn(tks);
                continue;
            case TkT::INCLUDE:
                new_lex(*tks[idx].data.txt);
                parse_file(*tks[idx].data.txt);
                lexidx--;
        }
    }
    #undef LEX
};
