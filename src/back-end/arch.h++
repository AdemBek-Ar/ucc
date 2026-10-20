enum struct ArchId : u16 {
	#define ARCH_KIND(TXT, NAME, CODE) NAME=CODE,
	#include "../../db/usm/arch.h"
	#undef ARCH_KIND
};
