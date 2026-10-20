/* ========================================================================== *
 | Cyver Command Line Argument DataBase Table                                 |
 | * this file hold operations alone for same spiciale definitions            |
 * ========================================================================== */

/*
CYVER_COMMAND(ARGC, NAME,         HELP_MSG,                                                  EXECUT_NAME    ) */
CYVER_COMMAND(0,    "*",          "Any command is not on this list is an unknown\n",         unknown    )

CYVER_COMMAND(0,    "--built-in", "Display built-in (types, mods, macros ...etc)",           built_in   )
CYVER_COMMAND(0,    "--version",  "Display version info",                                    version    )
CYVER_COMMAND(0,    "--help",     "Display help info\n",                                     help       )

CYVER_COMMAND(1,    "-abt",       "If no typ can hold literal, built speicel typ for it",    activeabt  )
CYVER_COMMAND(1,    "-ask",       "Fix error from CLI while compiling",                      activeask  )
CYVER_COMMAND(1,    "-arch",      "Set output architecture",                                 setarch    )
CYVER_COMMAND(1,    "-pkg",       "Set output package",                                      setpkg     )
CYVER_COMMAND(1,    "-tbs",       "Set tab by spaces",                                       settbs     )
CYVER_COMMAND(1,    "-o",         "Set output file\n",                                       setout     )

CYVER_COMMAND(0,    "-clean",     "Make a clean executable (only code, no info or symbols)", cleanout   )
CYVER_COMMAND(1,    "-l",         "Link a library (.a, .so, .dll ...etc)",                   link       )
CYVER_COMMAND(1,    "-#",         "Make a global value macro",                               rpl        )
CYVER_COMMAND(0,    "-shared",    "Make a shared library",                                   activeshred)
CYVER_COMMAND(1,    "-opt",       "Activate optimization",                                   activeopt  )
CYVER_COMMAND(0,    "-dbg",       "Activate debug info",                                     activedbg  )
CYVER_COMMAND(0,    "-nowrg",     "Disable warnings",                                        nowrg      )
CYVER_COMMAND(0,    "-s",         "Output assembly",                                         outasm     )
CYVER_COMMAND(0,    "-cstyl",     "C declaring stayle",                                      cstyl      )
