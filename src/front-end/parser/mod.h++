inline void parse_mod(TksView tks) {
    idx++;
    auto oldscope=scope;
    if (is_def(tks[idx].id)) scope=peek_ident(tks, scope);
    else scope=nullptr;
    if (istk_blk(tks[idx].id))
        handle_codeblk(tks, &Parser::parse);
    scope=oldscope;
}



inline void parse_pkg(TksView tks) {
    idx++;
    if (is_def(tks[idx].id)) {
        if (pkg.name) {
            LEX.pwrg("pkg has name", tks[idx-1].loc, tks[idx-1].len);
            if (!askfor("renaming pkg").empty()) goto setpkgname;
        }
        else setpkgname: pkg.name=peek_ident(tks, scope);
    }
    if (tks[idx].id==TkT::MOD) {
        idx++;
        if (is_def(tks[idx].id)) scope=peek_ident(tks, scope);
        else LEX.pwrg("identifire needed", tks[idx-1].loc, tks[idx-1].len);
    }
}
