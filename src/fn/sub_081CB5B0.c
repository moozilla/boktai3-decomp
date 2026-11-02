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

void sub_081CB5B0(void *ctx, struct SceneNpc *npc, s32 tick)
{
    if (npc->enter) {
        u32 zero = 0;
        npc->enter = zero;
        npc->done = zero;
        CpuSet(gPaletteBankData + 0x680, npc->palette, 0x04000008);
        Actor_PlayAnimationEntry(npc, 0, 2, 1, 4);
        npc->phase = zero;
        npc->stateFrame = zero;
    }
    switch (npc->phase) {
        case 0: {
            u32 t = npc->stateFrame;
            if (t <= 0x80) {
                void *bank1 = gPaletteBankData + 0x680;
                void *bank2 = gPaletteBankData + 0x2E80;
                void *dest = npc->palette;
                Palette_InterpolateBanks(dest, bank1, bank2, t, 7);
            } else {
                npc->stateFrame = 0;
                npc->phase++;
                PlayerParticleCloud_Create(&npc->position, 1);
                sub_081D3620();
                Sound_PlaySong(0x5B5);
            }
            break;
        }
        case 1: {
            {
                u32 mask = 0x100;
                npc->displayFlags |= mask;
            }
            if (tick > 0x12C) {
                CpuSet(gPaletteBankData + 0x680, npc->palette, 0x04000008);
                PlayerParticleCloud_RequestStop();
                npc->done = 1;
                {
                    void (*callback)(void) = Actor_OnHiddenIdleUpdate;
                    npc->prevState = 1;
                    npc->stateCallback = callback;
                }
                npc->stateFrame = 0;
                npc->enter = 1;
                npc->phase = 0;
            }
            break;
        }
    }
}
