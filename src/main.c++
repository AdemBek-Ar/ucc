/* ========================================================================== *
||   BETA                                      |                             ||
||   ██████╗██╗   ██╗██╗   ██╗███████╗██████╗  | Uncomplited Cyver Compiler  ||
||  ██╔════╝╚██╗ ██╔╝██║   ██║██╔════╝██╔══██╗ | version 0.9.0 beta          ||
||  ██║      ╚████╔╝ ██║   ██║█████╗  ██████╔╝ | MIT ©️ 05/09/2026           ||
||  ██║       ╚██╔╝  ╚██╗ ██╔╝██╔══╝  ██╔══██╗ |                             ||
||  ╚██████╗   ██║    ╚████╔╝ ███████╗██║  ██║ |                             ||
||   ╚═════╝   ╚═╝     ╚═══╝  ╚══════╝╚═╝  ╚═╝ |                             ||
||-------------------------------------------------------------------------- ||
|| * no IR base, direct compiling to C, assembly or binary                   ||
|| * this compiler accept files or folders as input                          ||
|| * self documented source code                                             ||
|| * no clang or LLVM backend                                                ||
|| * C macros database                                                       ||
 * ==========================================================================*/

#include "dependencies.h"


#include "rustayle.h++"  // rust type style (a shortcuts)




namespace cyv /* push all code in this namespace */ {
	/* these are one header mini librarys that
	    can't work alone, it should be include
	    here
	 */
	#include "front-end/tk.h++"
	#include "front-end/lex.h++"
	#include "front-end/tree.h++"
	#include "front-end/parser.h++"
	#include "back-end/gen.h++"
}


cyv::Parser parser;
cyv::Gen    gen;



#include "cmd.h++"

int main(int len, char* args[]) {
	if (len<2) {
		::perr("no input files, folders or flags");
		cmd::help();
		return 1;
	}

//     parser=cyv::Parser();
    parser.push_built_in();
	// reparsing command line args
	for (; cmdidx<(usz)len; cmdidx++) {
		if (args[cmdidx][0]=='-') cmdargs.push_back(args[cmdidx]);
		else parser.push_file(args[cmdidx]);   // push file or folder (package)
	}
	cmdidx=0;
	cmd::handle_command();

	if (int ec=parser.parse_all()) return ec;
	if (int ec=gen.gen(&parser)) return ec;
	return gen.out();
}
