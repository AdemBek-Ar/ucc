/* ========================================================================== *
 | Cyver CLI(nterface) absetraction layer (.h++)                              |
 | * this file hold  *list (string list of commands names)                    |
 |                   *help_ls (string list of commands help)                  |
 |                   *argc_ls (i8 list of commands argc)                      |
 |                   *cmds_len (length of all previos lists)                  |
 |                   *miniargs (global list for command arguments)            |
 |                   *cmd_vtable (the actions for each command)               |
 |                   *handle_command (cmd function that handle argv[i])       |
 * ========================================================================== */


std::vector<std::string> miniargs;
std::vector<std::string> cmdargs;
usz cmdidx=1;

namespace cmd {
	constexpr std::string_view list[]={
		#define CYVER_COMMAND(ac, cmd, help, FN) cmd,
		#include "../db/cli/cmd.h"
		#undef CYVER_COMMAND
	};
	constexpr std::string_view help_ls[]={
		#define CYVER_COMMAND(ac, cmd, help, FN) help,
		#include "../db/cli/cmd.h"
		#undef CYVER_COMMAND
	};
	constexpr i8 argc_ls[]={
		#define CYVER_COMMAND(ac, cmd, help, FN) ac,
		#include "../db/cli/cmd.h"
		#undef CYVER_COMMAND
	};
	constexpr usz cmds_len=sizeof(list)/sizeof(std::string_view);


	#define CYVER_COMMAND(ac, cmd, help, FN) void FN();
	#include "../db/cli/cmd.h"
	#undef CYVER_COMMAND

	typedef void (*Lmb)();
	constexpr Lmb cmd_vtable[]={
		#define CYVER_COMMAND(ac, cmd, help, FN) &FN,
		#include "../db/cli/cmd.h"
		#undef CYVER_COMMAND
	};



	// definition of commands actions
	void setpkg() { parser.pkg.name=parser.push_ident(cyv::Ident(parser.push_symbol(miniargs[0]), nullptr)); }
	void settbs() { parser.tbs=std::stoul(miniargs[0]); }
	void activeshred() { gen.shared=true; }
	void activeask() { parser.ask=true; }
	void cstyl() { parser.cstyl=true; }
	void nowrg() { parser.nowrg=true; }
	void cleanout() { gen.clean=true; }
    void activeabt() {parser.abt=true;}
	void activeopt() { gen.opt=true; }
	void activedbg() { gen.dbg=true; }


	void unknown() {
		::perr("unknown command, try:");
		help();
	}


	void setout() {
		if (!gen.output.empty()) {
			::perr("output file already provided `{}`", gen.output);
			return;
		}
		gen.output=miniargs[0];
	}


	void outasm() {
		gen.lang=cyv::Gen::ASM;
		setout();
	}


	void built_in() {
		std::println(
			#include "../db/cli/built-in.h"
		);
	}



	void version() {
		std::println(
			"cyv (CYV) 0.1 2026/06/18 Cyver compailer\n"
			"Copyright (C) 2026 (MIT version 2), Inc.\n"
			"This is free-to-use software\n");
	}


	void help() {
		::phlp("cyv [Flags], file... folder...");
		for (usz i=0; i<cmds_len; i++)
			std::println(" {:>10}   {}", list[i], help_ls[i]);
	}


	void setarch() {
		#define ARCH_KIND(TXT, NAME, CODE) \
			if (!miniargs[0].compare(TXT)) {\
				gen.arch=cyv::ArchId::NAME;\
				return;\
			}
		#include "../db/usm/arch.h"
		#undef ARCH_KIND
		::perr("no support for`{}`", miniargs[0]);
	}


	void link() {
		sfs::path libpath=miniargs[0];
		auto libfmt=libpath.extension();
		if (libfmt==".a" || libfmt==".o") gen.sll.insert(miniargs[0]);
		else if (libfmt==".so" || libfmt==".dll") gen.dll.insert(miniargs[0]);
		else ::perr("unknown library format");
	}


	void rpl() {
		auto name=parser.push_ident(cyv::Ident(parser.push_symbol(miniargs[0]), nullptr));
		cyv::Var macro;
		macro.loc=0;
		macro.consty=cyv::Consty::RPL;
		parser.new_lex(&miniargs[1]);
		auto tks=parser.lexs[parser.lexidx].tks();
		macro.val=parser.parse_exp(tks);
		if (macro.val && macro.val->consty!=cyv::Consty::RPL) {
			::perr("expect constexpration");
			goto quit;
		}
		{
			auto [it, ok]=parser.pkg.vars.insert({name, macro});
			if (!ok) {
				parser.redef
				(std::format("var `{}`", name->txt()), macro.loc, tks[parser.idx].loc-macro.loc+tks[parser.idx].len);
				goto quit;
			}
		}
		quit:
		parser.idx=0;
		parser.lexs.pop_back();
		parser.lexidx=0;
	}


	void handle_command() {
		if (cmdargs.empty()) return;
		std::string name; name.reserve(cmdargs[cmdidx].size());
		for(usz i=0; i<cmds_len; i++) {
			usz j=0;
			for (; j<list[i].size(); j++) {
				if (list[i][j]!=cmdargs[cmdidx][j]) {
					name="";
					goto _next_cmd;
				}
				name[j]=cmdargs[cmdidx][j];
			}
			name.shrink_to_fit();
			miniargs.clear();
			if (j<cmdargs[cmdidx].size()) {
				switch (cmdargs[cmdidx][j]) {
					case '=': {
						j++;
						get_args:
						usz idx=0;
						miniargs.push_back("");
						for (; j<cmdargs[cmdidx].size(); j++) {
							if (cmdargs[cmdidx][j]==',') {
								miniargs.push_back("");
								idx++;
							}
							miniargs[idx]+=cmdargs[cmdidx][j];
						}
					} break;
					default:
						if (argc_ls[i]>0) goto get_args;
						goto _next_cmd;
				}
			}
			miniargs.shrink_to_fit();
			if (miniargs.size()<(usz)argc_ls[i]) {
				::perr("few args");
				continue;
			}
			else if (miniargs.size()>(usz)argc_ls[i]) ::pwrg("too much args");
			cmd_vtable[i]();
			_next_cmd:
		}
	}
}
