#include "global.h"

struct P2 {
    u32 a, b;
};

struct Ent {
    u8 on;
    u8 kind;
    u8 pad2[2];
    u16 z4;
    u8 ang;
    u8 pad7;
    u8 b8;
    u8 pad9[3];
    u16 c;
    u8 padE[0x18 - 0xE - 2];
    s16 x;
    s16 pad1a;
    s16 y;
    s16 pad1e;
};

struct Mgr {
    u8 pad0[0x18];
    u32 flag18;
    u32 w1c;
    struct Ent ents[16];
};

struct Cmd {
    u8 pad[8];
    u16 id;
    s16 v;
};

struct Src {
    u8 pad[0xc];
    struct P2 pos;
};

s32 Math_Atan2(s32, s32);
struct Src *sub_08225984(int);

s32 sub_08010298(struct Mgr *p, u32 q, struct Cmd *cmd)
{
    s32 i;
    struct Src *s;
    s32 v;
    struct P2 pos;
    u8 *e;
    u16 *idp = (u16 *)((u8 *)p + 0xCA6);

    *idp = cmd->id;
    s = sub_08225984(*idp);
    if (s == 0)
        return -1;
    pos = *(struct P2 *)((u8 *)s + 0xc);
    v = cmd->v;
    p->w1c = 0;
    for (i = 15, e = (u8 *)p + 0x24; i >= 0; i--, e += 0xc8) {
        u32 n, n2;
        s32 ang;
        u32 t1, t2;
        vu16 *tbl;
        ((struct Ent *)e)->kind = 3;
        ((struct Ent *)e)->on = 1;
        ((struct Ent *)e)->z4 = 0;
        *(struct P2 *)(e + 0x18) = pos;
        ang = Math_Atan2(*(s16 *)((u8 *)e + 0x18) - *(s16 *)((u8 *)e + 0x10),
                         *(s16 *)((u8 *)e + 0x1c) - *(s16 *)((u8 *)e + 0x14));
        ((struct Ent *)e)->ang = ang;
        tbl = (vu16 *)0x0203B400;
        n = (*(u32 *)0x03005308 + 1) & 0x3FF;
        ((struct Ent *)e)->ang = ang - 16 + (tbl[n] & 0x1f);
        ((struct Ent *)e)->c = v;
        n2 = (n + 1) & 0x3FF;
        *(u32 *)0x03005308 = n2;
        ((struct Ent *)e)->b8 = (tbl[n2] & 7) + 7;
    }
    p->flag18 = 1;
    return 0;
}
