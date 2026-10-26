#include "global.h"
#include "engine.h"
#include "render.h"

struct NpcDisplayPalette {
    u8 unk_00[0xC];
    void *palette;
};

struct SceneNpc {
    u8 unk_00[6];
    u8 prevState, done, changed, enter;
    u8 unk_0A[4];
    u16 displayFlags;
    u32 stateFrame;
    u8 unk_14[4];
    u8 phase;
    u8 unk_19[0x68 - 0x19];
    struct {
        s16 x, y, z, pad;
    } position;
    u8 unk_70[0xD4 - 0x70];
    u8 animation[0xE0 - 0xD4];
    struct NpcDisplayPalette display;
    u8 unk_F0[0x1CC - 0xF0];
    u16 palette[16];
    u8 unk_1EC[0x2CC - 0x1EC];
    void (*stateCallback)(void);
};

void Palette_InterpolateBanks(void *, void *, void *, u32, u32);
s32 DisplayAnimation_TestFinished(void *);
u32 PlayerParticleCloud_Create(void *, u32);
s32 PlayerParticleCloud_RequestStop(void);
s32 sub_081D3620(void);
void Actor_OnHiddenIdleUpdate(void);

void SceneNpc_UpdateParticleDisappearance(void *ctx, struct SceneNpc *npc, s32 tick)
{
    if (npc->enter) {
        u32 zero = 0;
        npc->enter = zero;
        npc->done = zero;
        Actor_PlayAnimationEntry(npc, 0, 0, 8, 0);
        npc->phase = zero;
        npc->stateFrame = zero;
        CpuSet(gPaletteBankData + 0x2AE0, npc->palette, 0x04000008);
    }
    npc->displayFlags |= 1;
    switch (npc->phase) {
        case 0:
            if (npc->stateFrame <= 0x80) {
                void *a = gPaletteBankData + 0x2AE0;
                void *b = gPaletteBankData + 0x2B00;
                void *dest = npc->palette;
                Palette_InterpolateBanks(dest, a, b, tick, 7);
                {
                    struct NpcDisplayPalette *display = &npc->display;
                    display->palette = dest;
                }
            } else {
                npc->phase = 1;
                npc->stateFrame = 0;
            }
            break;
        case 1:
            if (DisplayAnimation_TestFinished(npc->animation)) {
                PlayerParticleCloud_Create(&npc->position, 0);
                npc->stateFrame = 0;
                npc->phase++;
                Sound_PlaySong(0x5B5);
            }
            break;
        case 2: {
            u32 mask = 0x100;
            npc->displayFlags |= mask;
        }
            sub_081D3620();
            npc->stateFrame = 0;
            npc->phase++;
            break;
        case 3: {
            u32 mask = 0x100;
            npc->displayFlags |= mask;
        }
            if (tick > 0xF0) {
                void (*callback)(void);
                CpuSet(gPaletteBankData + 0x2AE0, npc->palette, 0x04000008);
                PlayerParticleCloud_RequestStop();
                npc->done = 1;
                callback = Actor_OnHiddenIdleUpdate;
                npc->prevState = 1;
                npc->stateCallback = callback;
                npc->stateFrame = 0;
                npc->enter = 1;
                npc->phase = 0;
            }
            break;
    }
}
