#include "code.h++"

inline Def<Fn> *parse_fn(TksView tks) {
    Fn fn{};
    fn.lid=lexidx;
    fn.loc=tks[idx-1].loc;

    // get main name
    Ident *name=getfname(tks);
    if (!name) return nullptr;

    // get args
    if (istk_blk(tks[idx].id) && tks[idx].id!=TkT::TAB) {
        if (tks[idx].data.block->empty()) {
            idx++;
            goto mangling;
        }
        ovs.push_back(cvs);
        cvs=&fn.args;
        handle_codeblk(tks, &Parser::parse_exp, TkT::SIMICOLUMN);
        cvs=ovs[ovs.size()-1];
        ovs.pop_back();
    }
    mangling:
    // mangle the name with args
    name= mangle(name, fn.getypnames());

    if (tks[idx].id==TkT::ARROW) idx++;
    // getting typ
    fn.typ=peek_typ(tks);

    fn.len=tks[idx].loc-fn.loc;

    // parsing code block
    while (tks[idx].id==TkT::NEW_LINE) {
        idx++;
        if (idx>=tks.size()) goto set_undef;
    }
    if (istk_blk(tks[idx].id)) {
        if (tks[idx].data.block->empty()) {
            idx++;
            LEX.pwrg("empty fn block", fn.loc, fn.len);
            goto set_undef;
        }
        auto oldscope=scope;
        scope=name;
        ovs.push_back(&fn.args); // scope
        ovs.push_back(cvs);
        cvs=&fn.vars;
        fn.code=handle_codeblk(tks, &Parser::parse_code, &fn);
        if (!fn.code) {
            LEX.perr("empty fn block", fn.loc, fn.len);
            return nullptr;
        }
        
        fn.len=tks[idx].loc-fn.loc+tks[idx].len;

        cvs=ovs[ovs.size()-1];
        ovs.pop_back();
        ovs.pop_back();
        fn.status=Status::DEF;
        scope=oldscope;
    }
    else set_undef: fn.status=Status::UNDEF;

    // pushing the function
    return push_fn(name, fn);
}



inline Ident *getfname(TksView tks) {
    std::string name;
    if (is_def(tks[idx].id)) return peek_ident(tks);
    if (is_op(tks[idx].id) || istk_blk(tks[idx].id))
        name=std::format("{}", tkls[tks[idx++].n]);
    else {
        expect("ident, block or operator as fn name", tks[idx].loc, tks[idx].len);
        return nullptr;
    }
    if (name.empty()) return nullptr;
    return push_ident(Ident(push_symbol(name), scope));
}




inline Def<Fn> *parse_mainfn(TksView tks) {
    Fn fn{};
    fn.lid=lexidx;
    fn.loc=tks[idx-1].loc;

    // get main name
    Ident *name=push_ident(push_symbol("-->"), nullptr);
    if (!name) return nullptr;

    // parsing code block
    if (istk_blk(tks[idx].id)) {
        auto oldscope=scope;
        scope=name;

        ovs.push_back(cvs);
        cvs=&fn.vars;
        fn.code=handle_codeblk(tks, &Parser::parse_code, &fn);
        cvs=ovs[ovs.size()-1];
        ovs.pop_back();

        scope=oldscope;
        if (!fn.code) goto undef_status;
    }
    else undef_status: fn.status=Status::UNDEF;
    return push_fn(name, fn);
}
