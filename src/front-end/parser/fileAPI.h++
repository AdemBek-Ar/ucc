// push a file to parse it
inline void push_file(std::string txt) {
	sfs::path p=txt; // getting path as `sfs::path` onec so it no init with every sfs::fncall
	if (sfs::exists(txt)) /* check the existing */ {
		if (sfs::is_directory(p))             /* if it was a folder */ {
			auto inpaths=get_files_in(p);        // get files inside it
			for (usz i=0; i<inpaths.size(); i++) // and
				push_file(inpaths[i]);           // push them
		}
		else /* if it file just push it */
			files.push_back(txt);
		return;
	}
	::perr("no such file or directory \"{}\"", txt);
}



inline void search_undefs(){
	// a loop to get undefined Members
	for (auto it: pkg.fns) {
		if (it.second.status==Status::UNDEF) {
			std::string emsg=std::format
                ("undefined {} function `{}`", Consty_ls[u8(it.second.consty)], it.first->txt());
			lexs[it.second.lid].perr(emsg, it.second.loc, 3);
		}
	}
	for (auto it: pkg.typs) {
		if (it.second.status==Status::UNDEF) {
			std::string emsg=std::format("undefined {} `{}`", TKind_ls[u8(it.second.tk)], it.first->txt());
			lexs[it.second.lid].perr(emsg, it.second.loc, 3);
		}
	}
}



inline int parse_file(sfs::path file) {
	auto f_fmt=file.extension();
	if (f_fmt==".cyv") {
		auto tks=LEX.tks();
		#ifdef DBG
			for (auto tk:tks) tk.print();
			std::putchar('\n');
		#endif

		if (tks.empty()) /* if the file was empty */ {
			LEX.phlp("empty file", 0, 0, [](std::string_view line, usz loc, usz len) {
				return std::format(" {:>4} | {}-->{}{}// entry point{}\n {:>4} | \t{}{}//write code here{}",
					1, CLI::YELLOW, CLI::BASHED, CLI::DARKER, CLI::NON, 2,
					CLI::BASHED, CLI::DARKER, CLI::NON);
			});
			goto quit;
		}

		tks.push_back(Tk(TkT::_EOF, LEX.idx-1, 1)); /* so we don't need to check eof every single
													   time we move in reading */
		this->parse(tks);

		idx=0;
		return LEX.ec;
	}
	else if (f_fmt==".bkj")
		::pwrg("unimplemented package system. yet.");
	quit:return 0;
}



inline int parse_all() {
	int ec=0;
	for (auto file : files) {
		if (new_lex(file)) return 1;
		if (!LEX.fsrc) return LEX.ec;
		ec |= parse_file(file); // don't ask me about that
	}
	search_undefs();
	#ifdef DBG
		pkg.print();
	#endif
	return ec;
}
