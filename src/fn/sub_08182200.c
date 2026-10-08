#include "global.h"
struct T { void (*cb)(void); u32 b; u8 pad[0x8b8 - 8]; u16 c; };
struct S { u8 f[0x650]; struct T t; };
void sub_08182088(void);
void sub_08182200(u8 *s)
{
    struct T *t = (struct T *)(s + 0x650);
    switch (*(u8 *)(s + 0xb38)) {
    case 3: case 5: case 7: case 9: case 0xd: case 0xf:
        if (t->cb == 0) {
            t->cb = sub_08182088;
            t->b = 0;
            *(u16 *)((u8 *)t + 0x488) = 0;
        }
        break;
    default:
        break;
    }
}
