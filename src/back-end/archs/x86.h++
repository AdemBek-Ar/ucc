enum Instr {
	#define INSTER(txt, NAME, VAL, TXT) NAME=VAL
	#include "../../../db/usm/instr/x86.h"
	#undef INSTER
}
enum Rgst {
	#define INSTER(txt, NAME, VAL, TXT) NAME=VAL
	#include "../../../db/usm/rgst/x86.h"
	#undef INSTER
}
