inline Code *parse_code(TksView tks, Fn *fn) {
    if (tks.empty()) return nullptr;
    Code *code=new Code;
    for (; idx<tks.size() && tks[idx]; idx++) {
        if (auto stmt=parse_stmt(tks, fn, code->size())) {
            reset:
            if (istk_blk(stmt->id)) {
                if (stmt->blk) {
                    for (auto it:*stmt->blk)
                        code->push_back(it);
                    stmt->blk=nullptr;
                }
            }
            else {
                if (stmt->id==TkT::EF && (code->empty() || (*code)[code->size()-1]->id!=TkT::IF || (*code)[code->size()-1]->id!=TkT::EF))
                    stmt->id=TkT::IF;
                else if (stmt->id==TkT::EL && (code->empty() || (*code)[code->size()-1]->id!=TkT::IF || (*code)[code->size()-1]->id!=TkT::EF)) {
                    stmt->id=TkT::LBR;
                    goto reset;
                }
                code->push_back(stmt);
            }
        }
    }
    if (code->empty()) {
        delete code;
        code=nullptr;
    }
    else code->shrink_to_fit();
    return code;
}



inline Exp *parse_stmt(TksView tks, Fn *fn, usz stmt_idx=0) {
    if (idx>=tks.size()) goto nquit;
    switch (tks[idx].id) {
        // instructions that take unfinety args
        case TkT::SYS: {
            usz i=idx; idx++;
            auto exp=handle_codeblk(tks, &Parser::parse_exp, TkT::SIMICOLUMN);
            if (!exp) goto nquit;
            exp->id=tks[i].id;
            return exp;
        }
        // instructions that take 1 numarical args
        case TkT::POP:
        case TkT::PUSH:
        case TkT::INT:
        case TkT::BREAK:
        case TkT::SKP:
        case TkT::JMP: {
            Exp *exp=new Exp(tks[idx++]);
            exp->opl=handle_codeblk(tks, &Parser::parse_exp, TkT::SIMICOLUMN);
            if (!exp->opl) {
                if (exp->id==TkT::BREAK || exp->id==TkT::SKP) return exp;
                Exp::operator delete(exp);
                goto nquit;
            }
            if (!exp->opl->typ.pc && exp->opl->typ.row->second.tk!=TKind::ENM) {
                LEX.perr("only registers, vars, numarical values", tks[idx].loc, tks[idx].len);
                Exp::operator delete(exp);
                goto nquit;
            }
            return exp;
        }
        // instructions that take 1 numarical args
        case TkT::RET: {
            Exp *exp=new Exp(tks[idx++]);
            exp->opl=handle_codeblk(tks, &Parser::parse_exp, TkT::SIMICOLUMN);
            if (!exp->opl) {
                Exp::operator delete(exp);
                goto nquit;
            }
            return exp;
        }
        // instructions that take 2 args
        case TkT::CMP:
        case TkT::MV: {
            usz i=idx; idx++;
            auto exp=handle_codeblk(tks, &Parser::parse_exp, TkT::SIMICOLUMN);
            if (!exp) goto nquit;
            if (exp->id!=TkT::COMMA) {
                expect("2 args at lease", tks[idx].loc, tks[idx].len);
                Exp::operator delete(exp);
                goto nquit;
            }
            exp->id=tks[i].id;
            return exp;
        }
        // instructions that take 2 code blocks
        case TkT::EF:
        case TkT::IF: {
            idx++;
            Exp *exp=new Exp(tks[idx++]);
            if (istk_blk(tks[idx].id)) {
                exp->cond=handle_codeblk(tks, &Parser::parse_exp, TkT::SIMICOLUMN);
                idx++;
                break;
            }
            else {
                expect("condetion block", tks[idx-1].loc, tks[idx-1].len);
                Exp::operator delete(exp);
                return nullptr;
            }
            if (!exp->cond) {
                LEX.perr("faild to parse condetion", tks[idx].loc, tks[idx].len);
                goto retnon;
            }
            if (!exp->cond->typ.pc && exp->cond->typ.row->second.tk!=TKind::ENM) goto not_a_condition;
            if (exp->cond->consty==Consty::RPL) {
                switch (exp->cond->id) {
                    case TkT::CHAR_LIT:
                        if (exp->cond->data.c==0) goto retnon;
                        goto set_as_block;
                    case TkT::FLOAT_LIT:
                        if (*exp->cond->data.f==0) goto retnon;
                        goto set_as_block;
                    case TkT::NUM_LIT:
                        if (*exp->cond->data.i==0) retnon: {
                            Exp::operator delete(exp);
                            return nullptr;
                        }
                        set_as_block: exp->id=TkT::LBR;
                        break;
                    default: not_a_condition:
                        LEX.perr("not a condetion", tks[idx].loc, tks[idx].len);
                        goto retnon;
                }
            }

            auto oldscope=scope; {
                std::string *label=push_symbol(std::format("{} {}", tkls[exp->n], stmt_idx));
                scope= push_ident(Ident(label, scope));
            }
            exp->code=handle_codeblk(tks, &Parser::parse_code, fn);
            scope=oldscope;
            if (!exp->code) {
                LEX.perr("faild to parse block", tks[idx].loc, tks[idx].len);
                goto nquit;
            }
            return exp;
        }
        default:
            return parse_exp(tks, TkT::SIMICOLUMN);
    }
    nquit: return nullptr;
}