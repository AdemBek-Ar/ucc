// 1:typ[SIZE] name
// 2:   let member type=default_value
inline Def<Typ> *parse_typ(TksView tks) {
    Typ typ;
    typ.lid=lexidx;
    typ.loc=tks[idx-1].loc;
    switch (tks[idx-1].id) /* setting type kind */ {
        case TkT::TYP: typ.tk=TKind::TYP; break;
        case TkT::ENM: typ.tk=TKind::ENM; break;
        case TkT::UNI: typ.tk=TKind::UNI; break;
        default: return nullptr;
    }
    if (tks[idx].id==TkT::LPR) /* setting type size */  {
        Exp *exp=handleblk(tks, &Parser::parse_exp, TkT::SIMICOLUMN);
        if (!exp) goto continue_;
        if (exp->consty==Consty::NON) {
            LEX.perr("not a rpl expr", tks[idx].loc, tks[idx].len);
            goto continue_;
        }
        if (exp->id!=TkT::NUM_LIT && exp->id!=TkT::CHAR_LIT) {
            LEX.perr("not a numarical expr", tks[idx].loc, tks[idx].len);
            goto continue_;
        }
        if (exp->id==TkT::CHAR_LIT) {
            if (exp->data.c < 0) {
                LEX.perr("typ size can't be < 0", tks[idx].loc, tks[idx].len);
                goto continue_;
            }
            typ.size=exp->data.c*8;
        }
        else {
            if ((*exp->data.i) < 0) {
                LEX.perr("typ size can't be < 0", tks[idx].loc, tks[idx].len);
                goto continue_;
            }
            typ.size=exp->data.i->get_ui()*8;
        }
        continue_: idx++;
    }

    bool apply_alignment=true;
    for (; idx<tks.size(); idx++) {
        switch (tks[idx].id) {
            case TkT::NOT:
                apply_alignment=false;
                continue;
            case TkT::DOT:
                typ.isflt=true;
            case TkT::MINUS:
                typ.sign=true;
                continue;
            case TkT::MUL:
                typ.isptr=true;
                continue;
            case TkT::POINTS:
                typ.istxt=true;
                continue;
            case TkT::FN:
                typ.tk=TKind::FN;
                continue;
            default: goto quit_addotionals_loop;
        }
        break;
    } quit_addotionals_loop:

    // getting name
    if (!is_def(tks[idx].id)) {
        expect("ident for typ name", tks[idx].loc, tks[idx].len);
        return nullptr;
    }


    Ident *name=peek_ident(tks, scope);
    if (!name) return nullptr;
    // handle redefinition
    auto [it, ok]=pkg.typs.insert({name, typ});
    auto typ_=const_cast<Def<Typ>*>(&*it);
    if (typ_->second.is_gen) {
        typ_->second=typ;
        goto getting_block;
    }
    if (!ok) {
        redef(std::format("typ `{}`", name->txt()), typ.loc, tks[idx].loc-typ.loc+tks[idx].len);
        return nullptr;
    }
    getting_block:
    switch (tks[idx].id) {
        case TkT::NEW_LINE:
        case TkT::SIMICOLUMN:
            if (typ_->second.size>0) typ_->second.status=Status::OVLD;
            else typ_->second.status=Status::UNDEF;
            break;
        case TkT::DO:
        case TkT::RET:
        case TkT::LB:
        case TkT::LBR:
        case TkT::LPR:
        case TkT::TAB:
        case TkT::TKWOWT: {
            {
                auto oldscope=scope;
                scope=name;
                ovs.push_back(cvs);
                cvs=&typ_->second.vars;
                handle_codeblk(tks, &Parser::parse);
                cvs=ovs[ovs.size()-1];
                ovs.pop_back();
                scope=oldscope;
            }
            typ_->second.status=Status::DEF;
            usz rts=0;
            for (auto [name, var]: typ_->second.vars) {
                if (var.consty==Consty::NON) {
                    if (var.typ.arr || var.typ.pc!=0 || var.typ.opt) rts+=PTRSIZE*8;
                    else rts+=var.typ.row->second.size;
                }
            }
            if (typ_->second.size && rts>typ_->second.size)
                LEX.pwrg(std::format("typ size is bigger then provided, `{} > {}`", rts, typ_->second.size),
                    tks[idx].loc, tks[idx].len);
            typ_->second.size=rts;
            idx--;
        } break;
        default:
            expect("block", tks[idx].loc, tks[idx].len);
            return nullptr;
    }

    if (apply_alignment) typ_->second.alignment();
    return typ_;
}



inline TypHolder compatable(TypHolder typ0, TypHolder typ1) {
    if (typ0.arr) {
        typ0.arr=false;
        typ0.pc+=typ0.apc+1;
    }
    if (typ1.arr) {
        typ1.arr=false;
        typ1.pc+=typ1.apc+1;
    }

    // enm with enm operation
    if (typ0.row->second.tk==TKind::ENM && typ1.row->second.tk==TKind::ENM && typ0.pc<=0 && typ1.pc<=0) {
        // if (op==TkT::AS || op==TkT::TO) return typ1;
        if (typ1.row->second.size>typ0.row->second.size) return typ1;
        return typ0;
    }

    // ptr casting
    if (typ0.pc>0 && typ1.pc>0) {
        if (typ0.row==typ1.row) {
            if (typ0.pc>typ1.pc) return typ0;
            return typ1;
        }
        // if (op==TkT::AS || op==TkT::TO) typ1;
        return TypHolder{}; // can't cast to a ptr of other typ
    }

    // enm with ptr operation -> like C, returning a ptr
    if ((typ0.row->second.tk==TKind::ENM && typ1.pc>0)|| (typ1.row->second.tk==TKind::ENM && typ0.pc>0)) {
        if (typ0.pc>0) return typ0;
        return typ1;
    }

    return TypHolder{};
}



TypHolder match(TkT op, TypHolder typ0, TypHolder typ1, bool switchable=false) /*find operations between typs*/ {
    // if (auto is=compatable(typ0, typ1)) return is;
    // find operation definition
    auto defn=getfn(
        push_ident(push_symbol(std::format("{}", tkls[u8(op)])), typ0.row->first),
        {typ1.row->first});
    if (!defn && switchable)
        defn=getfn(
            push_ident(push_symbol(std::format("{}", tkls[u8(op)])), typ1.row->first),
            {typ0.row->first});
    if (!defn) return TypHolder{};
    return defn->second.typ;
}



TypHolder typ_match(TypHolder typ0, TypHolder typ1) /*find initualazing and casting between typs*/ {
    if (typ0==typ1) return typ0;
    if (auto is=compatable(typ0, typ1)) return is;
    // typ0 constractors from typ1 or reverse
    if (auto mtch=match(TkT::FRM, typ0, typ1, true)) return mtch;
    // typ0 casting from typ1 or reverse
    if (auto mtch=match(TkT::TO,  typ0, typ1, true)) return mtch;
    // typ0 literal-casting from typ1 or reverse
    if (auto mtch=match(TkT::AS,  typ0, typ1, true)) return mtch;
    return TypHolder{};
}



bool can_hold(TypHolder typdef, Exp *exp) /*if typ can hold an expretion or a literal value*/ {
    if (exp->typ) return typdef==exp->typ || typ_match(exp->typ, typdef);
    if (is_op(exp->id)) {
        if (exp->opl && exp->opr && exp->opl->typ && exp->opr->typ) {
            if (exp->typ=match(exp->id, exp->opl->typ, exp->opr->typ)) return true;
            if (exp->typ=compatable(exp->opl->typ, exp->opr->typ)) return true;
        }
        bool ret=true;
        if (exp->opl) ret= ret && can_hold(typdef, exp->opl);
        if (exp->opr) ret= ret && can_hold(typdef, exp->opr);
        if (ret) exp->typ=typdef;
        return ret;
    }
    if (typdef.pc!=0 || typdef.arr) {
        LEX.perr("array, ref or ptr can't hold literals", exp->loc, exp->len);
        return false;
    }
    bool ret=false;
    switch (exp->id) {
        case TkT::CHAR_LIT:  ret= typdef.row->second.tk==TKind::ENM && typdef.row->second.fit_for(utf8_len(exp->data.c)); break;
        case TkT::NUM_LIT:   ret= typdef.row->second.tk==TKind::ENM && typdef.row->second.fit_for(exp->data.i); break;
        case TkT::FLOAT_LIT: ret= typdef.row->second.tk==TKind::ENM && typdef.row->second.fit_for(exp->data.f); break;
        case TkT::RG_LIT:    ret= typdef.row->second.fit_for(exp->data.rgsz/8); break;
        case TkT::TXT_LIT:   ret= typdef.row->second.fit_for(exp->data.txt); break;
        // case TkT::BOOL_LIT:  return typdef.row->second.fit_for(1);
        default: return false;
    }
    if (ret) exp->typ=typdef;
    return ret;
}




TypHolder typcheck(Exp *exp) /*find the expretion typ*/ {
    if (istk_lit(exp->id)) {
        if (exp->typ) return exp->typ;
        if (!exp->typ) {
            if (!search_for_fityp(exp)) {
                LEX.perr("untyped literal", exp->loc, exp->len);
                return TypHolder{};
            }
            return exp->typ;
        }
    }
    if (is_op(exp->id) && !istk_usmkw(exp->id)) {
        TypHolder a, b;
        if (exp->opl) {
            if (exp->opl->typ) a=exp->opl->typ;
            else {
                a=exp->opl->typ=typcheck(exp->opl);
                if (!a) goto no_typ_expside;
            }
        }
        if (exp->opr) {
            if (exp->opr->typ) b=exp->opr->typ;
            else {
                b=exp->opr->typ=typcheck(exp->opr);
                if (!b) goto no_typ_expside;
            }
        }

        if (!exp->opl) {
            switch (exp->id) {
                case TkT::AND:
                    b.pc++;
                case TkT::AND_SIGN:
                    b.pc++;
                    return b;
                case TkT::POW:
                    b.pc--;
                case TkT::MUL:
                    b.pc--;
                    return b;
                default: break;
            }
        }
        if (a || b) return a? a:b; // one of the sides are NON or void

        // both type
        if (auto retyp=match(exp->id, a, b)) return retyp;
        if (auto retyp=typ_match(a, b)) return retyp;
        LEX.perr("typ can't hold literal", exp->loc, exp->len);
        return TypHolder{};
    }
    switch (exp->id) {
        case TkT::LET:
        case TkT::VARCALL: // holding variable
            LEX.perr("can't get var typ", exp->var->second.loc, 1);
            return TypHolder{};
        case TkT::FN:
        case TkT::FNCALL:  // holding function
            if (exp->fn) return exp->fn->second.typ;
            LEX.perr("can't get fn typ", exp->fn->second.loc, 1);
            return TypHolder{};
        default:
            LEX.perr("typ error", exp->loc, exp->len);
            return TypHolder{};
    }
    no_typ_expside:
        LEX.perr("exp need typ", exp->loc, exp->len);
        return TypHolder{};
}



inline bool search_for_fityp(Exp *exp) {
    u8 i=0;
    for (auto &typ:pkg.typs) {
        if (i>=BUILT_IN_TYP_COUNT) break;
            
        switch (exp->id) {
            #define RETHIS \
                exp->typ=TypHolder{};\
                exp->typ.row=&typ;\
                return true
            case TkT::NUM_LIT:
                if (typ.second.tk==TKind::ENM && typ.second.fit_for(exp->data.i)) {
                    if (((*exp->data.i)<0 && typ.second.sign) || !typ.second.sign) {
                        RETHIS;   
                    }
                }
                goto continue_;
            case TkT::FLOAT_LIT:
                if (typ.second.tk==TKind::ENM && typ.second.isflt && typ.second.fit_for(exp->data.f)) { RETHIS; }
                goto continue_;
            case TkT::CHAR_LIT: // all sort are covred
                if (typ.second.tk==TKind::ENM && typ.second.sign && typ.second.fit_for(utf8_len(exp->data.c))) { RETHIS; }
                goto continue_;
            case TkT::RG_LIT:   // all sort are covred
                if (typ.second.tk==TKind::ENM && typ.second.fit_for(exp->data.rgsz/8)) { RETHIS; }
                goto continue_;
            case TkT::TXT_LIT:  // all sort are covred
                if (typ.second.istxt && typ.second.fit_for(exp->data.txt)) { RETHIS; }
            #undef RETHIS
            continue_:
                i++;
                continue;
            // case TkT::BOOL_LIT:  return typ.second.fit_for(1);
            default: return false;
        }
    }

    Typ typ{};
    typ.is_gen=true;
    typ.loc=exp->loc;
    std::string name;
    switch (exp->id) {
        case TkT::CHAR_LIT: // numarical with to big size
            typ.tk=TKind::ENM;
            typ.size= utf8_len(exp->data.c)*8;
            typ.sign=true;
            name+='i';
            break;

        case TkT::NUM_LIT: // numarical with to big size
            typ.tk=TKind::ENM;
            typ.size= mpz_sizeinbase(exp->data.i->get_mpz_t(), 2);
            if ((*exp->data.i)<0) {
                typ.sign=true;
                name+='i';
            }
            else name+='u';
            break;

        case TkT::FLOAT_LIT: // decimal with to big size
            typ.tk=TKind::ENM;
            typ.size= exp->data.f->get_prec()+7;
            typ.sign=true;
            typ.isflt=true;
            name+='f';
            break;

        default: return false;
    }
    typ.alignment();
    name+=std::to_string(typ.size);
    exp->typ=TypHolder{};
    colored_print(CLI::BLUE, "typ `{}` generated for literal in\n", name);
    exp->typ.row=push_typ(push_ident(name, nullptr), typ);
    LEX.ploc(exp->loc, exp->len, CLI::BLUE);
    return true;
}
