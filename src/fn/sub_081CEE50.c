// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"

struct SceneNpc081CEE50 {
    u8 pad0[6];
    u8 kind, done, pad8, entered;
    u8 padA[2];
    u16 flagsC, flagsE;
    u32 unk10;
    u8 pad14[4];
    u8 phase, bit1, bit2, angle;
    u8 pad1C[4];
    struct {
        u8 pad[0xA];
        u16 hA;
    } sub20;
    u8 pad2C[0x65 - 0x2C];
    u8 angle65;
    u8 pad66[0xD4 - 0x66];
    u8 anim[0x2CC - 0xD4];
    void *cb2CC;
    u8 pad2D0[0x2E4 - 0x2D0];
    void *callback;
};

s32 DisplayAnimation_TestFinished(void *);
s32 Callback_InvokeActor(void *, void *);
void Actor_OnIdleUpdate(void);

// a pinned to r2; the shift temp and mask registers still swap
void sub_081CEE50(void *ctx, struct SceneNpc081CEE50 *npc)
{
    typeof(npc->sub20) *sub = &npc->sub20;
    u32 play = 0;
    if (npc->entered) {
        s32 a;
        u32 two;
        u32 f;
        npc->entered = 0;
        npc->done = 0;
        a = npc->angle;
        asm("" : "+r"(a));
        npc->angle65 = a;
        {
            register s32 t asm("r1") = (a + 0x70) >> 5;
            u32 m = 7;
            npc->bit2 = m & t;
        }
        {
            s32 t = (a + 0x60) >> 6;
            u32 m;
            m = 3;
            m &= t;
            npc->bit1 = m;
        }
        Actor_PlayAnimationEntry(npc, 4, 2, 0, 4);
        two = 2;
        f = npc->flagsC;
        if (!(two & f))
            play = 1;
        {
            s32 three = 3;
            npc->flagsC = f & -three;
        }
        {
            u32 mask = 0x100;
            npc->flagsE = mask | npc->flagsE;
        }
        sub->hA = 0;
    }
    if ((u8)Callback_InvokeActor(npc, npc->callback) != 1) {
        u32 one, zero;
        if (play)
            Sound_PlaySong(0x11E);
        one = 1;
        zero = 0;
        npc->flagsE |= one;
        if (DisplayAnimation_TestFinished(npc->anim)) {
            void *cb;
            npc->done = one;
            Actor_PlayAnimationEntry(npc, 0, 1, 0, 4);
            cb = Actor_OnIdleUpdate;
            npc->kind = 2;
            npc->cb2CC = cb;
            npc->unk10 = 0;
            npc->entered = one;
            npc->phase = zero;
            if (!(npc->flagsC & one)) {
                Sound_PlaySong(0x120);
                npc->flagsC = ~one & npc->flagsC;
            }
        }
    }
}
