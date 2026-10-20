inline Def<Var> *parse_macro(TksView tks) {
    Var var{};
    var.lid=lexidx;
    var.consty=Consty::RPL;
    var.loc=tks[idx-1].loc;
    if (!is_def(tks[idx].id)) {
        expect("ident as macro name", tks[idx].loc, tks[idx].len);
        // ASK for name
        return nullptr;
    }
    auto name=peek_ident(tks, scope);
    if (!name) return nullptr; // `peek_ident` already print that error

    if (is_initop(tks[idx].id)) {
        idx++;
        if (istk_blk(tks[idx].id)) {
            var.tks=tks[idx].data.block;
            idx++;
        }
        else {
            expect("macro block", tks[idx].loc, tks[idx].len);
            return nullptr;
        }
    }

    return push_var(name, var);
}
