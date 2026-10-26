#include "global.h"
#include "engine.h"
struct Vec {
    u16 x, y, z, pad;
};
struct Pair {
    u32 a;
    u32 b;
};
struct SceneNpc {
    u8 pad0[7];
    u8 done, changed, entered;
    u8 padA[4];
    u16 flags;
    u8 pad10[0x10];
    void *effect;
    u8 pad24[0x68 - 0x24];
    struct Vec pos;
    u8 pad70[0xE0 - 0x70];
    struct {
        u8 pad[0xC];
        void *paletteDest;
    } sub;
    u8 padF0[0x1CC - 0xF0];
    u8 palette[0x20];
    u8 pad1EC[0x2E4 - 0x1EC];
    u8 (*callback)(struct SceneNpc *);
};
void *SceneNpc_CreateTransitionEffect(struct Vec *);
s32 SceneNpc_IsTransitionEffectFinished(void *);
void SceneNpc_DestroyTransitionEffect(void *);
void Palette_InterpolateBanks(void *, void *, void *, u32, u32);
// A statement macro (do/while(0)) around the fade call, as its loop weight decides the registers.
#define FadePaletteBanks(dest, from, to, step)                                                     \
    do {                                                                                           \
        Palette_InterpolateBanks(dest, from, to, step, 5);                                         \
    } while (0)
// confidence=low: SceneNpc_UpdateTransitionFade's palette fade alone: copies the start bank on
// entry, fades to the end bank over ticks 0x50-0x6F, and shows the end bank while the
// callback reports 1.
void sub_081CF398(void *ctx, struct SceneNpc *npc, s32 tick)
{
    u32 fade;
    if (npc->entered) {
        npc->entered = 0;
        npc->done = 0;
        CpuSet(gPaletteBankData + 0x2B00, npc->palette, 0x04000008);
        Actor_PlayAnimationEntry(npc, 7, 1, 0, 1);
    }
    if (npc->callback(npc) == 1) {
        u8 *subp = (u8 *)npc + 0xE0;
        *(void **)(subp + 0xC) = gPaletteBankData + 0x2AE0;
        return;
    }
    if (tick == 0) {
        CpuSet(gPaletteBankData + 0x2B00, npc->palette, 0x04000008);
    } else {
        fade = tick - 0x50;
        if (fade <= 0x1F) {
            u8 *a = gPaletteBankData + 0x2B00;
            u8 *b = gPaletteBankData + 0x2AE0;
            FadePaletteBanks(npc->palette, a, b, fade);
        } else if (tick == 0x70) {
            CpuSet(gPaletteBankData + 0x2AE0, npc->palette, 0x04000008);
        }
    }
    {
        u8 *subp = (u8 *)npc + 0xE0;
        *(void **)(subp + 0xC) = npc->palette;
    }
}
