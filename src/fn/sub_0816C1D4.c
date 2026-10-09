#include "global.h"
struct E1D4 { u8 f[1]; u8 value; u8 tail[14]; };
struct S1D4 { u8 f0[0x20]; u32 arg; u8 f1[0x48f8 - 0x24]; struct E1D4 *entries; u8 f2[0x4904 - 0x48fc]; u8 index; u8 state; };
void sub_082164AC(u32, u32, u32);
void sub_0816BA90(struct S1D4 *);
void sub_0816BC54(struct S1D4 *);
void sub_0816BB10(struct S1D4 *);
void sub_0816C1A4(struct S1D4 *);
void sub_0816C1D4(struct S1D4 *s) {
    if (s->state == 1) {
        sub_082164AC(1, s->arg, s->entries[s->index].value);
        sub_0816BA90(s);
        sub_0816BC54(s);
    } else {
        sub_0816C1A4(s);
        sub_0816BB10(s);
        sub_0816BC54(s);
    }
}
