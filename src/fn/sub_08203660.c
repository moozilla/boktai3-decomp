#include "global.h"
struct Args {
    s32 x, y, z, a, b, c, d, e;
};
struct Message {
    u32 type : 16;
    u32 reserved : 16;
    struct Args *args;
};
extern u8 *gGameState;
void Script_Call(u32, struct Message *);
void GameState_SetEventFlag(s32);
void sub_08159164(s32);
#define H(p, n) (*(s16 *)((p) + (n)))
#define W(p, n) (*(u32 *)((p) + (n)))
static inline s32 clamp(s32 v)
{
    if (v > 9999)
        v = 9999;
    else if (v < 0)
        v = 0;
    return v;
}
#define INC(n) H(gGameState, n) = clamp(H(gGameState, n) + 1)
void EnemyF426_RecordDefeat(u8 *p)
{
    struct Args a;
    struct Message m;
    u32 q = W(p, 0x14C);
    if (q) {
        a.x = H(p, 0x40);
        a.y = H(p, 0x42);
        a.z = H(p, 0x44);
        a.a = W(p, 0x168);
        a.b = W(p, 0x16C);
        a.c = W(p, 0x170);
        a.d = W(p, 0x160);
        a.e = W(p, 0x15C);
        m.type = 8;
        m.args = &a;
        Script_Call(q, &m);
    }
    GameState_SetEventFlag(28);
    if (W(p, 0x160) & 1) {
        u16 flags;
        sub_08159164(H(p, 0x158));
        H(gGameState, 0x87C) = H(gGameState, 0x87C) < 9999 ? H(gGameState, 0x87C) + 1 : 9999;
        H(gGameState, 0x538) = H(gGameState, 0x538) < 9999 ? H(gGameState, 0x538) + 1 : 9999;
        flags = H(p, 0x13A);
        H(gGameState, 0x574) = H(gGameState, 0x574) < 9999 ? H(gGameState, 0x574) + 1 : 9999;
        if (flags & 0x80) {
            INC(0x58A);
        } else if (flags & 0x100) {
            INC(0x58C);
        } else if (flags & 0x200) {
            INC(0x58E);
        } else if (flags & 0x400) {
            INC(0x590);
        } else if (flags & 0x800) {
            INC(0x592);
        }
    }
}
