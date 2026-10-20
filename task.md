# Done
## lexer level
- [X] implementing numarical literals
- [X] implementing float literals
- [X] implementing text literals
- [X] implementing char literals
- [X] implementing register literals
- [X] implementing key words
- [X] implementing assembly key words
- [X] implementing identefires
- [X] implementing backslash pre-prossecing
- [X] implementing operations
- [X] implementing blocks \{\(\[\`\\t\`\]\)\}
- [X] implementing operation hashing
- [X] implementing custemazing numbers base `0h hex` `0b bin` `0o octal` `0x custom`

## parser level
- [X] implementing identefire parsing
- [X] implementing module namespacing
- [X] implementing setting package name
- [X] implementing general 8byte Flags complix struct
- [X] implementing putting package in a mod
- [X] implementing typ parsing
- [X] implementing setting typ size
- [X] implementing mini-flags to make typ able to hold literals
- [X] implementing auto typ generation for literals when not find one
- [X] fix redefining generated typ
- [X] implementing var declaration
- [X] implementing pointer typ
- [X] implementing referance typ
- [X] implementing optional typ
- [X] implementing mutable typ
- [X] implementing const typ
- [X] implementing owned typ
- [X] implementing array typ
- [X] implementing auto typ gussing for a var declaring
- [X] implementing typ checking
- [X] fix typ matching
- [X] fix typ to/frm typ
- [X] edit Consty
- [X] implementing constexpr solving
- [X] fix constexpr solving
- [X] fix constexpr typ setting
- [X] fix Parser::slove(Exp*)
- [X] redesign macros handling

## generator level
- [X] implementing C generator
- [X] shaping ELF64 header
- [X] shaping ELF32 header
- [X] shaping section64 header
- [X] shaping section32 header
- [X] shaping segment64 header
- [X] shaping segment32 header



# Not yet
- [ ] implementing version and address for 
- [ ] implementing function typ
- [ ] implementing tuple typ
- [ ] compliting C generator
- [ ] implementing x86-64bit assembly generator
- [ ] implementing x86-64bit binary generator

- [ ] rewrite cyver in it self
- [ ] optimazing
- [ ] implementing optimazation
- [ ] implementing pkg system
- [ ] implementing complix numbers from literal level
- [ ] implementing mathamatical typs generator
- [ ] implementing non crapy project config system
- [ ] implementing version literals

- [ ] implementing x86-32bit assembly generator
- [ ] implementing arm-64bit binary generator
- [ ] implementing arm-32bit binary generator
- [ ] implementing RSVIC binary generator
- [ ] implementing linkable.o

- [ ] implementing machine level programing
- [ ] savety roles
- [ ] implementing C/C++ parsing headers
- [ ] implementing Rust parsing headers

- [ ] implementing QBE generation
- [ ] implementing WASM generation
- [ ] implementing LLVM IR generation


# ohter projects (depende on this one)
- [ ] ucc standarad library
- [ ] (T/G)UI library (yoUIy)
- [ ] no dependence on libc (ucc.a only)
- [ ] making custem graphical 2D/3D API (vviper)
- [ ] making a phisics library (haytem)
- [ ] making a game library (Noengin) // that's the name btw
- [ ] remaking PS2 game for linux with cyv1, with respicting LICENS
