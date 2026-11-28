#include "global.h"
#include "engine/engine.h"
#include "engine/video/render.h"
#include "npc/story_character/state_actor.h"

void Palette_InterpolateBanks(void *, void *, void *, u32, u32);
s32 DisplayAnimation_TestFinished(void *);
u32 PlayerParticleCloud_Create(void *, u32);
s32 PlayerParticleCloud_RequestStop(void);
s32 PlayerParticleCloud_ScatterAnchors(void);
void Actor_OnHiddenIdleUpdate(void);

void sub_081CB5B0(void *ctx, struct StateActor *npc, s32 tick)
{
    if (npc->enter) {
        npc->enter = 0;
        npc->done = 0;
        CpuSet(gPaletteBankData + 0x680, npc->overlayDrawable[0].palette, 0x04000008);
        Actor_PlayAnimationEntry(npc, 0, 2, 1, 4);
        npc->phase = 0;
        npc->stateFrame = 0;
    }
    switch (npc->phase) {
        case 0: {
            u32 t = npc->stateFrame;
            if (t <= 0x80) {
                u8 *from = gPaletteBankData + 0x680, *to = gPaletteBankData + 0x2E80;
                Palette_InterpolateBanks(npc->overlayDrawable[0].palette, from, to, t, 7);
            } else {
                npc->stateFrame = 0;
                npc->phase++;
                PlayerParticleCloud_Create(&npc->position, 1);
                PlayerParticleCloud_ScatterAnchors();
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
                CpuSet(gPaletteBankData + 0x680, npc->overlayDrawable[0].palette, 0x04000008);
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
