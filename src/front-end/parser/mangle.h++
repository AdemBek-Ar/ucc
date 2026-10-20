/*=======================================================================*
 t he function and operato*rs mangling incoding are following these rols
 - compressing addtional info (arg type name, scope, ...etc)
 - repleacing long type names with Indexs, < MAX_TYPNAME_LEN chars
 - allowing symbols that aren't alphabite or numbers
 * operators are prefixed with `op` + optxt
 - spaces are allowed
 - saving main name
 *=======================================================================*/

/*** example
 * ─fn main(argc i32, argv **ch8) int
 └ ─> `main i32 ch8`    *
 *─op +(a u8, b u8) u8
 └─> `op+ u8 u8`

 for long types names:
     cpp::slop::slop::long::f___ing::standared::name
     they get indexed with a hash function then ingected into the name
     ***/

#define MAX_TYPNAME_LEN 11
#define PRIME_SEED 991343


static std::string minitxthash(std::string txt, usz seed=PRIME_SEED) {
    // getting full typname and hash iapply_manglet so that it starting with a non alpha char
    std::string ret=std::to_string(txt.size());
    usz hash=seed;
    for (usz i=0; i<txt.size(); i++)
        hash += (txt[i] << i) * seed;
    ret+=std::to_string(hash);
    return ret;
}

inline std::string *typname_mangle(Ident *name) {
    if (!name) return nullptr;
    auto txt=name->txt();
    if (txt.size()<MAX_TYPNAME_LEN) return push_symbol(txt);
    return push_symbol(minitxthash(txt));
}


Ident *mangle(Ident* name, std::vector<Ident*> typnames) {
    std::string basename=*name->name;
    for (auto typname:typnames)
        if (auto typn=typname_mangle(typname))
            basename=std::format("{} {}", basename, *typn);
    return push_ident(Ident(push_symbol(basename), name->scp));
}
