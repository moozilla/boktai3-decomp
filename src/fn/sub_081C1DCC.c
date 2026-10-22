#include "global.h"
void Script_Call(u32, void *);
struct A { u16 a; u16 b; void *ptr; };
void sub_081C1DCC(u8 *p)
{
    u32 script = *(u32 *)(p + 0x5a8);
    if (script) {
        u32 buf;
        struct A args;
        args.a = 1;
        args.ptr = &buf;
        Script_Call(script, &args);
    }
}
