// handling var declaration
Def<Var> *parse_let(TksView tks) {
    Var var{};
    var.lid=lexidx;
    // setting location
    var.loc=tks[idx-1].loc;
    // check whether the var is runtime or compiletime
    if (idx>1 && tks[idx-1].id==TkT::LET && tks[idx-2].id==TkT::HASH) var.consty=Consty::CTO;

    if (!is_def(tks[idx].id)) /* if no name found */ {
        expect("ident as name", tks[idx].loc, tks[idx].len);
        // TODO: ASK for name
        return nullptr;
    }

    auto name=peek_ident(tks, scope); // getting name
    if (!name) return nullptr;        // `peek_ident` already print that error

    if (tks[idx].id==TkT::POINTS) idx++; // just stayle, but need when typ is in global scope

    var.typ=peek_typ(tks); // peek up the typ if found

    // setting first or default value, needed for consts & compiletime optimazation
    if (is_initop(tks[idx].id)) {
        idx++;
        var.val=parse_exp(tks);       // getting the expretion
        if (!var.val) {
            if (var.consty!=Consty::NON) {
                LEX.perr("need an expretion", var.loc, 3);
                return nullptr;
            }
            goto push_decl; //  skip if faild to parse it, parse_exp will mark an error
        }
        if (var.val->consty!=Consty::NON) var.val->mcro_exp=true;
        
        // if it was an void expretion
        if (!var.val->typ) {
            LEX.perr("non typed expretion", var.val->loc, var.val->len);
            return nullptr;
        }
        
        // auto typ gussing
        if (!var.typ) {
            var.typ=var.val->typ;
            goto dret;
        }
        
        // typ matching
        if(auto finaltyp=typ_match(var.typ, var.val->typ))
            var.typ=var.val->typ=finaltyp;
        else {
            LEX.perr("typ didn't match", var.val->loc, var.val->len);
            return nullptr;
        }
    }

    push_decl:
    if (!var.typ) {
        if (defaultyp) /* if there is a default typ */ {
            var.typ=defaultyp;
            goto dret;
        }
        LEX.perr("can't get typ", var.loc, tks[idx].loc+tks[idx].len-var.loc);
        return nullptr;
    }
    // push var into current scope
    dret: return push_var(name, var);
}
