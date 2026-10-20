inline int genc(Parser *parser) {
    int ec=0;
    {
        std::vector<std::pair<Ident* const, Typ>> typs(parser->pkg.typs.begin(), parser->pkg.typs.end());
        for (auto it=typs.rbegin(); it!=typs.rend(); it++) {
            switch (it->second.tk) {
                case TKind::ENM:
                    switch (it->second.status) {
                        case Status::UNDEF:
                        case Status::OVRD:
                            continue;
                        case Status::OVLD: {
                            #define CBIFT(NAME, SIZE)\
                                case SIZE: {\
                                    auto [tyit, ok]=parser->pkg.typs.insert\
                                        ({const_cast<Ident*>(it->first), Typ{}});\
                                    (*const_cast<Ident**>(&tyit->first))=\
                                        parser->push_ident(Ident(parser->push_symbol(NAME),nullptr));\
                                } break
                            if (it->second.isflt) {
                                switch (it->second.size) {
                                        CBIFT("_Float16",16);
                                        CBIFT("float",32);
                                        CBIFT("_Float64",64);
                                        CBIFT("__float128",128);
                                        CBIFT("__float256",256);
                                        CBIFT("__float512",512);
                                    default:
                                        parser->lexs[it->second.lid].perr
                                            (std::format("no C float with size=`{}`", it->second.size/8), it->second.loc, 3);
                                        ec=1;
                                        continue;
                                }
                            }
                            else {
                                std::string unsignable="";
                                if (!it->second.sign) {
                                    if (it->second.size>=128) unsignable="u";
                                    else unsignable="unsigned ";
                                }
                                switch (it->second.size) {
                                    CBIFT(std::format("{}char", unsignable),8);
                                    CBIFT(std::format("{}short", unsignable),16);
                                    CBIFT(std::format("{}int", unsignable),32);
                                    CBIFT(std::format("{}long", unsignable),64);
                                    CBIFT(std::format("__{}int128_t", unsignable),128);
                                    CBIFT(std::format("__{}int256_t", unsignable),256);
                                    CBIFT(std::format("__{}int512_t", unsignable),512);
                                    default:
                                        parser->lexs[it->second.lid].perr
                                            (std::format("no C numaric with size=`{}`", it->second.size/8), it->second.loc, 3);
                                        ec=1;
                                        continue;
                                }
                            }
                            #undef CBIFT
                        } break;
                        case Status::DEF: {
                            emit("enum ");
                            emit(it->first->txt("___"));
                            emit('{');
                            emit('\n');
                            for (auto jt=it->second.vars.begin(); jt!=it->second.vars.end(); jt++) {
                                if (jt->second.consty==Consty::RPL)
                                    genc_enmname(parser, const_cast<Def<Var>*>(&*jt));
                                else {
                                    parser->lexs[it->second.lid].perr("C enums can't has members", it->second.loc, 3);
                                    ec=1;
                                }
                            }
                            emit("};\n");
                        }
                    }
                    break;
                case TKind::UNI:
                case TKind::TYP:
                    genc_typ(const_cast<Def<Typ>*>(&*it));
                case TKind::FN:
                    parser->lexs[it->second.lid].pwrg("unsupported yet", it->second.loc, 3);
            }
        }
    }{
        std::vector<std::pair<Ident* const, Var>> vars(parser->pkg.vars.begin(), parser->pkg.vars.end());
        for (auto it=vars.rbegin(); it!=vars.rend(); it++) {
            if (it->second.consty==Consty::NON) {
                genc_var(const_cast<Def<Var>*>(&*it));
                emit(";\n");
            }
            else genc_macro(const_cast<Def<Var>*>(&*it));
        }
    }
    return ec;
}



inline int genc_enmname(Parser *parser, Def<Var> *var) {
    int ec=0;
    emit(var->first->txt("___"));
    if (var->second.val) {
        emit('=');
        if (var->second.val->id==TkT::NUM_LIT)
            emit(var->second.val->data.i->get_str(10));
        else {
            parser->lexs[parser->lexidx].perr("C enums can only hold numarical values", var->second.loc, 3);
            ec=1;
        }
    }
    emit(',');
    return ec;
}



inline int genc_var(Def<Var> *var) {
    emit(var->second.typ.row->first->txt("___"));
    emit(' ');
    emit(*var->first->name);
    if (var->second.val) {
        emit('=');
        genc_exp(var->second.val);
    }
    return 0;
}

inline int genc_macro(Def<Var> *var) {
    emit("#define ");
    emit(var->first->txt("___"));
    if (var->second.val) genc_exp(var->second.val);
    return 0;
}



inline int genc_typ(Def<Typ> *typ) {
    switch (typ->second.status) {
        case Status::UNDEF:
        case Status::OVRD:
            return 0;
        case Status::OVLD:
        case Status::DEF:
            if (typ->second.tk==TKind::TYP) emit("struct ");
            else emit("union ");
            emit(typ->first->txt("___"));
        emit('{');
        emit('\n');
        for (auto it=typ->second.vars.begin(); it!=typ->second.vars.end(); it++) {
            if (it->second.consty==Consty::RPL) emit("const ");
            genc_var(const_cast<Def<Var>*>(&*it));
            emit(";\n");
        }
        emit("};\n");
    }
    return 0;
}



inline int genc_exp(Exp *exp) {
    if (is_op(exp->id)) {
        #define SIDE(OP)\
            if (exp->OP) {\
                int ec=genc_exp(exp->OP);\
                if(ec) return ec;\
            }
            SIDE(opl)
            emit(std::string(tkls[exp->n]));
            SIDE(opr)
        #undef SIDE
        return 0;
    }
    switch (exp->id) {
        default:
            return 3;
        // case TkT::LET:
        //     genc_var(exp->var);
        //     emit(";\n");
        //     break;
        case TkT::NUM_LIT:
            emit(exp->data.i->get_str(10));
            break;
        case TkT::CHAR_LIT:
            emit('\'');
            emit(exp->data.c);
            emit('\'');
            break;
        case TkT::TXT_LIT:
            emit('"');
            emit(*exp->data.txt);
            emit('"');
            break;
        case TkT::FLOAT_LIT:{
            gmp::exp _exp;
            emit(exp->data.f->get_str(_exp, 10));
            break;
        case TkT::VARCALL:
            emit(exp->var->first->txt("___"));
            break;
        case TkT::DOT:
            emit(exp->opl->var->first->txt("___"));
            emit('.');
            emit(*exp->opr->var->first->name);
            break;
        case TkT::FNCALL:
            emit(exp->fn->first->txt("___"));
            break;
        }
    }
    return 0;
}
