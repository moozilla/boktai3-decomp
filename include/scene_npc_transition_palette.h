#ifndef GUARD_SCENE_NPC_TRANSITION_PALETTE_H
#define GUARD_SCENE_NPC_TRANSITION_PALETTE_H

#include "engine.h"

struct SceneNpcTransitionPosition {
    u16 x, y, z, pad;
};

struct SceneNpcPaletteBinding {
    u8 unknown0[0xC];
    void *paletteDest;
};

struct SceneNpcTransitionActor {
    u8 active, unknown1;
    u8 phase, step, mode, unknown5, state;
    u8 done, changed, entered;
    u8 padA[4];
    u16 flags;
    u32 stateTimer, modeTimer;
    u8 unknown18[8];
    void *effect;
    u8 pad24[0x68 - 0x24];
    struct SceneNpcTransitionPosition pos;
    u8 pad70[0xE0 - 0x70];
    struct SceneNpcPaletteBinding binding;
    u8 padF0[0x1CC - 0xF0];
    u8 palette[0x20];
    u8 unknown1EC[0x21B - 0x1EC];
    u8 scriptedMode;
    u8 unknown21C[0x2E4 - 0x21C];
    u8 (*callback)(struct SceneNpcTransitionActor *);
};

void Palette_InterpolateBanks(void *, void *, void *, u32, u32);

static inline void SceneNpc_ApplyTransitionPalette(struct SceneNpcTransitionActor *npc, s32 tick)
{
    u32 fade;
    if (tick == 0) {
        CpuSet(gPaletteBankData + 0x2B00, npc->palette, 0x04000008);
    } else {
        fade = tick - 0x50;
        if (fade <= 0x1F) {
            u8 *start = gPaletteBankData + 0x2B00;
            u8 *end = gPaletteBankData + 0x2AE0;
            Palette_InterpolateBanks(npc->palette, start, end, fade, 5);
        } else if (tick == 0x70) {
            CpuSet(gPaletteBankData + 0x2AE0, npc->palette, 0x04000008);
        }
    }
}

#endif
