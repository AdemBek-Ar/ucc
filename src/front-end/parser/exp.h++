inline bool finish_exp(TkT id) {
    switch (id) {
        case TkT::NEW_LINE: // is it \n
            if (idx>0) switch (id) /* and it not the first in the block */ {
                case TkT::COMMA: return false; // if comma     befor it goback to init
                default: break;            // else
            }
        case TkT::SIMICOLUMN: //idx++;
        case TkT::_EOF      : return true; // quit
        default: break;
    }
    return false;
}



inline Exp *init_exp(TksView tks, TkT end=TkT::SIMICOLUMN) {
    if (tks.empty() || idx >= tks.size()) return nullptr;
    Exp *exp=new Exp;
    exp->loc=tks[idx].loc;
    for (usz i=0; i<2 && idx<tks.size();) {
        if (finish_exp(tks[idx].id) || tks[idx].id==end) goto quit;

        if (istk_lit(tks[idx].id)) {
            exp->ops[i]=new Exp(tks[idx]);
            exp->ops[i]->consty=Consty::RPL;
            goto continue_;
        }
        
        if (is_def(tks[idx].id)) {
            exp->ops[i]=new Exp(tks[idx]);
            exp->ops[i]->ident=peek_ident(tks);
            if (!exp->ops[i]->ident) {
                Exp::operator delete(exp->ops[i]);
                exp->ops[i]=nullptr;
                goto continue_;
            }
            if (is_expblock(tks[idx].id)) {
                exp->ops[i]->id=TkT::FNCALL;
                exp->ops[i]->args=handleblk(tks, &Parser::parse_exp, end);
            }
            else exp->ops[i]->id=TkT::VARCALL;
            exp->len=tks[(idx<tks.size())? idx:idx-1].loc-exp->loc;
            i++;
            continue;
        }
        
        if (is_expblock(tks[idx].id)) {
            usz i=idx;
            exp->ops[i]=handleblk(tks, &Parser::parse_exp, end);
            if (exp->ops[i]) {
                exp->ops[i]->isblk=true;
                exp->len=tks[(idx<tks.size())? idx:idx-1].loc-exp->loc;
            }
            goto continue_;
        }
        
        if (is_op(tks[idx].id)) {
            if (tks[idx].id==TkT::AS) {
                idx++;
                exp->typ=peek_typ(tks);
                continue;
            }
            if (exp->n==0) {
                exp->id=tks[idx].id;
                i=0;
            }
        }
        continue_:
            idx++;
            i++;
            continue;
        unexpected(tks[idx].loc, tks[idx].len);
    }
    quit:
    if (exp && exp->n==0) {
        if (exp->opl && exp->opr && exp->opl->id==TkT::NUM_LIT) {
            exp->id=TkT::MUL;
            goto dquit;
        }
        if (!exp->opl) {
            Exp::operator delete(exp);
            exp=nullptr;
            goto dquit;
        }
        auto op=exp->opl;
        exp->opl=nullptr;
        Exp::operator delete(exp);
        exp=op;
    }
    dquit: return exp;
}



Exp *sort_exp(Exp *exp0, Exp *exp1) /* should never be giving a nullptr */ {
    if (exp1->opl || exp1->isblk) {
        if (exp0->opr || exp0->isblk) {
            LEX.perr("this operation is full, you may miss the operator", exp1->opl->loc, exp1->opl->loc-exp1->opr->loc);
            // ASK for operator
            return new Exp(TkT::COMMA, exp0->loc, exp0, exp1);
        }
        exp0->opr=exp1;
        return exp0;
    }
    if (exp0->id <= exp1->id) {
        exp1->opl=exp0->opr;
        exp0->opr=exp1;
        exp0->findpos();
        return exp0;
    }
    exp1->opl=exp0;
    exp1->findpos();
    return exp1;
}




Exp *parse_exp(TksView tks, TkT end=TkT::SIMICOLUMN) {
    Exp *exp0=init_exp(tks, end);
    if (finish_exp(tks[idx].id)) goto init_prop;
    while (Exp *exp1=init_exp(tks, end)) {
        exp0=sort_exp(exp0, exp1);
        if (finish_exp(tks[idx].id) || tks[idx].id==end) {
            idx++;
            break;
        }
    }
    init_prop:
    if (!exp0) return nullptr;
    // search for Vars, Fns, members
    if (!find_calls(exp0)) {
        Exp::operator delete(exp0);
        exp0=nullptr;
        return exp0;
    }
    // solve if constexpr
    if (solve(&exp0))
        exp0->consty=Consty::RPL;
    exp0->typ=typcheck(exp0);
    if (!exp0->typ) {
        Exp::operator delete(exp0);
        return nullptr;
    }
    return exp0;
}



bool find_calls(Exp *exp) {
    if (!exp) return true;
    if (exp->id==TkT::DOT || exp->id==TkT::ARROW) {
        if (exp->opl->id!=TkT::VARCALL) {
            expect("var", exp->opl->loc, exp->opl->len);
            return false;
        }
        if (!find_calls(exp->opl)) return false;
        if (exp->opl->typ.pc<=0 && exp->id==TkT::ARROW) 
            LEX.pwrg("`->` for non ptr", exp->opl->loc, exp->opl->len);
        auto oldscope=scope;
        auto ocvs=cvs;
        cvs=&exp->opl->var->second.typ.row->second.vars;
        scope=exp->opl->var->second.typ.row->first;
        if (!find_calls(exp->opr)) {
            scope=oldscope;
            cvs=ocvs;
            return false;
        }
        scope=oldscope;
        cvs=ocvs;
        return true;
    }
    if (is_op(exp->id))
        return find_calls(exp->opl) && find_calls(exp->opr);

    switch (exp->id) {
        case TkT::VARCALL:{
            auto name=push_ident(exp->ident->name, scope);
            exp->var=getvar(name);
            if (!exp->var) {
                undef(std::format("var `{}`", name->txt()), exp->loc, exp->len);
                return false;
            }
            exp->consty=exp->var->second.consty;
            exp->typ=exp->var->second.typ;
            break;
        }
        case TkT::FNCALL:{
            auto name=exp->ident;
            exp->fn=getfn(name, {});
            if (!exp->fn) {
                Fn fn;
                fn.status=Status::UNDEF;
                exp->fn=push_fn(name, fn);
            }
            exp->consty=exp->fn->second.consty;
            exp->typ=exp->fn->second.typ;
        }
        default: return true;
    }
    return true;
}



bool solve(Exp **exp_) {
    Exp *exp= *exp_;
    if (istk_lit(exp->id)) {
        if (exp->id==TkT::NUM_LIT) {
            usz req= mpz_sizeinbase(exp->data.i->get_mpz_t(), 2);
            if (req<=sizeof(ch32)*8) {
                exp->id=TkT::CHAR_LIT;
                exp->data.c=(ch32)exp->data.i->get_si();
            }
        } return true;
    }

    if (exp->id==TkT::VARCALL) {
        switch (exp->var->second.consty) {
            case Consty::CTO:
            case Consty::NON: {
                auto var=exp->var;
                Exp::operator delete(exp);
                *exp_=var->second.val;
                return true;
            }
            case Consty::RPL: {
                auto var=exp->var;
                Exp::operator delete(exp);
                usz i=idx; idx=0;
                *exp_=parse_exp(*var->second.tks);
                idx=i;
                return true;
            }
        }
    }

    if (exp->consty==Consty::RPL) return true;
    if (is_op(exp->id)) {
        if (exp->opl) {
            if (!solve(&exp->opl)) return false;
            exp->opl->consty=Consty::RPL;
        }
        if (exp->opr) {
            if (!solve(&exp->opr)) return false;
            exp->opr->consty=Consty::RPL;
        }

        // [X] -     CHR   -> CHR
        // [X] -     NUM   -> NUM
        // [X] -     FLOAT -> FLOAT
        // [X] CHR   CHR   -> CHR
        // [X] NUM   NUM   -> FLOAT|NUM|CHR
        // [X] FLOAT FLOAT -> FLOAT|NUM|CHR
        // [X] TXT   TXT   -> TXT

        // [ ] FLOAT CHR   -> FLOAT|CHR
        // [ ] FLOAT NUM   -> FLOAT|NUM
        // [ ] FLOAT TXT   -> TXT

        // [ ] NUM   CHR   -> CHR  |NUM
        // [ ] NUM   FLOAT -> FLOAT|NUM
        // [ ] NUM   TXT   -> TXT

        // [ ] CHR   FLOAT -> FLOAT|CHR
        // [ ] CHR   NUM   -> CHR  |NUM
        // [ ] CHR   TXT   -> TXT

        // [ ] TXT   CHR   -> TXT
        // [ ] TXT   NUM   -> TXT
        // [ ] TXT   FLOAT -> TXT

        if (exp->opl && exp->opr) {
            if (exp->opl->id==exp->opr->id) {
                switch (exp->opl->id) {
                #define OPC(TKT) case TkT::TKT:
                #define LD(D) exp->opl->data. D
                #define RD(D) exp->opr->data. D
                    case TkT::CHAR_LIT: {
                        ch32 result;
                        switch (exp->id) {
                            #include "../../../db/lit_op/CHR_CHR->CHR.h"
                        }
                        Exp::operator delete(exp->opl=nullptr);
                        Exp::operator delete(exp->opr=nullptr);
                        exp->id=TkT::CHAR_LIT;
                        exp->data.c=result;
                        return true;
                    }
                    case TkT::NUM_LIT: {
                        TkT id=TkT::NUM_LIT;
                        gmp::ibig iv;
                        gmp::fbig fv;
                        switch (exp->id) {
                            #include "../../../db/lit_op/NUM_NUM->NUM|FLOAT.h"
                        }
                        Exp::operator delete(exp->opl=nullptr);
                        Exp::operator delete(exp->opr=nullptr);
                        exp->id=id;
                        if (id==TkT::FLOAT_LIT) exp->data.f=push_num(fv);
                        else {
                            usz req= mpz_sizeinbase(iv.get_mpz_t(), 2);
                            if (req<=sizeof(ch32)*8) {
                                exp->id=TkT::CHAR_LIT;
                                exp->data.c=(ch32)iv.get_si();
                            }
                            else exp->data.i=push_num(iv);
                        }
                        return true;
                    }
                    case TkT::FLOAT_LIT: {
                        gmp::fbig result;
                        switch (exp->id) {
                            #include "../../../db/lit_op/FLOAT_FLOAT->NUM|FLOAT.h"
                        }
                        Exp::operator delete(exp->opl=nullptr);
                        Exp::operator delete(exp->opr=nullptr);

                        gmp::ibig iv;
                        gmp::fbig_to_ibig(iv.get_mpz_t(), result.get_mpf_t());
                        if (result==iv) {
                            usz req= mpz_sizeinbase(iv.get_mpz_t(), 2);
                            if (req<=sizeof(ch32)*8) {
                                exp->id=TkT::CHAR_LIT;
                                exp->data.c=(ch32)iv.get_si();
                            }
                            else {
                                exp->id=TkT::NUM_LIT;
                                exp->data.i=push_num(iv);
                            }
                        }
                        else {
                            exp->id=TkT::FLOAT_LIT;
                            exp->data.f=push_num(result);
                        }
                        return true;
                    }
                    case TkT::TXT_LIT: {
                        std::string txt;
                        switch (exp->id) {
                            case TkT::PLUS_EQ:
                            case TkT::PLUS:
                                txt= *exp->opl->data.txt + *exp->opr->data.txt;
                            default:
                                LEX.perr("unsupported operation", exp->loc, exp->len);
                                return false;
                        }
                        Exp::operator delete(exp->opl=nullptr);
                        Exp::operator delete(exp->opr=nullptr);
                        exp->id=TkT::TXT_LIT;
                        exp->data.txt=push_symbol(txt);
                        return true;
                    }
                    default: goto quit;
                #undef OPC
                #undef LD
                #undef RD
                }
            }
        }
        else if (exp->opr) {
            switch (exp->id) {
                case TkT::PLUS: return true;
                case TkT::MINUS:
                    switch (exp->opr->id) {
                        case TkT::CHAR_LIT:{
                            auto c=exp->opr->data.c;
                            Exp::operator delete(exp->opr=nullptr);
                            exp->id=TkT::CHAR_LIT;
                            exp->data.c=-c;
                        } break;
                        case TkT::FLOAT_LIT:{
                            auto f=exp->opr->data.f;
                            Exp::operator delete(exp->opr=nullptr);
                            exp->id=TkT::FLOAT_LIT;
                            exp->data.f=push_num(-(*f));
                        } break;
                        case TkT::NUM_LIT:{
                            auto i=*exp->opr->data.i;
                            i=-i;
                            Exp::operator delete(exp->opr=nullptr);
                            exp->id=TkT::NUM_LIT;
                            exp->data.i=push_num(i);
                        } break;
                        default:
                            LEX.perr("unsupported operation", exp->loc, exp->len);
                            return false;
                    } break;

                case TkT::NOT:
                    switch (exp->opr->id) {
                        case TkT::CHAR_LIT:{
                            auto c=exp->opr->data.c;
                            Exp::operator delete(exp->opr=nullptr);
                            exp->id=TkT::CHAR_LIT;
                            exp->data.c=!c;
                        } break;
                        case TkT::FLOAT_LIT:{
                            auto f=*exp->opr->data.f;
                            f=!f;
                            Exp::operator delete(exp->opr=nullptr);
                            exp->id=TkT::FLOAT_LIT;
                            exp->data.f=push_num(f);
                        } break;
                        case TkT::NUM_LIT:{
                            auto i=*exp->opr->data.i;
                            i=!i;
                            Exp::operator delete(exp->opr=nullptr);
                            exp->id=TkT::NUM_LIT;
                            exp->data.i=push_num(i);
                        } break;
                        default:
                            LEX.perr("unsupported operation", exp->loc, exp->len);
                            return false;
                    } break;
                default:
                    LEX.perr("unsupported operation", exp->loc, exp->len);
                    return false;
            }
            return true;
        }
    }
    quit:
    return false;
}
