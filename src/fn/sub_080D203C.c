#include "global.h"
s32 Mod(s32, s32);
void SceneActorAnimation_SelectDirectionalEntry(u8 *, s32);
void SceneActorAttachment_ReplaceResponseSlot(u8 *, s32, s32, u32);
void Golem_UpdateDeath_080D211C(void);
struct Sub {
    u8 f0[8];
    u16 h8;
    u8 fA[4];
    u16 hE;
};
struct Ctx {
    u8 f0[0x48];
    struct Sub sub;
};
inline static u8 TakeFlag(u8 *flag)
{
    if (*flag) {
        *flag = 0;
        return 1;
    }
    return 0;
}

void Mummy_UpdateDying(u8 *s, s32 arg)
{
    struct Sub *c = &(*(struct Ctx **)(s + 4))->sub;
    u8 *part = s + 0x3D4;
    s32 zero;
    u32 mask;
    s32 off;
    if (TakeFlag(s + 0x2B2)) {
        u32 test;
        u16 *flags;
        u32 value;
        SceneActorAnimation_SelectDirectionalEntry(s, 8);
        test = 2;
        flags = (u16 *)(s + 0x2D4);
        value = *flags;
        test &= value;
        if (test == 0) {
            test = 1;
            test |= value;
            *flags = test;
        }
    }
    if (Mod(arg, 60) == 0) {
        SceneActorAttachment_ReplaceResponseSlot(s, 2, 0, s[0x98]);
    }
    off = 0xF9;
    off <<= 1;
    if ((*((s16 *)(part + off))) <= 0) {
        mask = 0x80;
        mask <<= 5;
        off += 0xDE;
        zero = (*((u32 *)(s - (-off)))) & mask;
        if (zero == 0) {
            void (*fn)(void) = Golem_UpdateDeath_080D211C;
            u32 code = 6;
            u32 *flags;
            u32 clear;
            s[0x2B2] = 1;
            s[0x2B0] = zero;
            s[0x2B5] = code;
            *((void (**)(void))(s + 0x21C)) = fn;
            *((u16 *)(s + 0x2C8)) = zero;
            flags = (u32 *)(s + 0x114);
            clear = ~1;
            *flags &= clear;
            *((u32 *)(s + 0x104)) = zero;
            return;
        }
    }
    if (c->h8 == 2)
        c->hE = 0;
}
