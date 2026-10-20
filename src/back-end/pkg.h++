int Pkg::save() {
    if (!sfs::create_directory("./.ucc")) {
        ::perr("failed to make pkg-dir `./.ucc`");
        return 1;
    }

    std::FILE *of=nullptr;
    const char* path;
    if (name) path=std::format("./.ucc/{}.bkj", name->txt()).c_str();
    else      path=std::format("./.ucc/-unk-pkg.bkj").c_str();
    of=std::fopen(path);
    if (!of) {
        ::perr("failed to open file `{}`", path);
        return 2;
    }

    return 0;
}
