#include "global.h"
struct Args {
    s32 x, y, z, a, b, c, d, e;
};
struct Message {
    u32 type : 16;
    u32 reserved : 16;
    struct Args *args;
};
extern u8 *gUnk_02000710;
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
#define INC(n) H(gUnk_02000710, n) = clamp(H(gUnk_02000710, n) + 1)
void PinkSpine_RecordDefeat(u8 *p)
{
    struct Args a;
    struct Message m;
    u32 q = W(p, 0x158);
    if (q) {
        a.x = H(p, 0x40);
        a.y = H(p, 0x42);
        a.z = H(p, 0x44);
        a.a = W(p, 0x14C);
        a.b = W(p, 0x150);
        a.c = W(p, 0x154);
        a.d = W(p, 0x144);
        a.e = W(p, 0x140);
        m.type = 8;
        m.args = &a;
        Script_Call(q, &m);
    }
    GameState_SetEventFlag(2);
    if (W(p, 0x144) & 1) {
        sub_08159164(H(p, 0x13C));
        H(gUnk_02000710, 0x87C) =
            H(gUnk_02000710, 0x87C) < 9999 ? H(gUnk_02000710, 0x87C) + 1 : 9999;
        H(gUnk_02000710, 0x538) =
            H(gUnk_02000710, 0x538) < 9999 ? H(gUnk_02000710, 0x538) + 1 : 9999;
        H(gUnk_02000710, 0x56E) =
            H(gUnk_02000710, 0x56E) < 9999 ? H(gUnk_02000710, 0x56E) + 1 : 9999;
        INC(0x58A);
    }
}
