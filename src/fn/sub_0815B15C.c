#include "global.h"
struct Pos {
    u32 a, b;
};
void sub_0815AD78(u8 *);
void sub_0813AFEC(u8 *, s32, s32);
void sub_0814FFA4(void);
/* Scripted actor: target position *pos (or its own, +0x30) into +0x590, with
   c, b, a and 0x96 in +0x5A4/+0x59A/+0x59D/+0x59B; enters state 6 with update
   sub_0814FFA4. */
void PlayerCue_MoveTo(u8 *e, struct Pos *pos, s32 a, s32 b, u32 c)
{
    if (pos == 0)
        *(struct Pos *)(e + 0x590) = *(struct Pos *)(e + 0x30);
    else
        *(struct Pos *)(e + 0x590) = *pos;
    *(u32 *)(e + 0x5A4) = c;
    e[0x59A] = b;
    e[0x59D] = a;
    e[0x59B] = 0x96;
    sub_0815AD78(e);
    sub_0813AFEC(e, 6, 0);
    *(void **)(e + 0x58C) = sub_0814FFA4;
}
