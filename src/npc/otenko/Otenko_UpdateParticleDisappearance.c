#include "global.h"
#include "engine/engine.h"
#include "engine/video/render.h"
#include "npc/otenko/otenko_actor.h"

void Palette_InterpolateBanks(void *, void *, void *, u32, u32);
s32 DisplayAnimation_TestFinished(struct DisplayAnimation *);
u32 PlayerParticleCloud_Create(void *, u32);
s32 PlayerParticleCloud_RequestStop(void);
s32 PlayerParticleCloud_ScatterAnchors(void);
void Actor_OnHiddenIdleUpdate(void);

void Otenko_UpdateParticleDisappearance(void *ctx, struct OtenkoActor *npc, s32 tick)
{
    if (npc->animationEntered) {
        npc->animationEntered = 0;
        npc->done = 0;
        Actor_PlayAnimationEntry(npc, 0, 0, 8, 0);
        npc->animationPhase = 0;
        npc->stateFrame = 0;
        CpuSet(gPaletteBankData + 0x2AE0, npc->workPalette, 0x04000008);
    }
    npc->displayFlags |= 1;
    switch (npc->animationPhase) {
        case 0:
            if (npc->stateFrame <= 0x80) {
                u8 *from = gPaletteBankData + 0x2AE0, *to = gPaletteBankData + 0x2B00;
                u16 *palette = npc->workPalette;
                Palette_InterpolateBanks(palette, from, to, tick, 7);
                {
                    struct SceneAnimation *graphics = &npc->display.resource.sequence.graphics;
                    graphics->palette = palette;
                }
            } else {
                npc->animationPhase = 1;
                npc->stateFrame = 0;
            }
            break;
        case 1:
            if (DisplayAnimation_TestFinished(&npc->display)) {
                PlayerParticleCloud_Create(&npc->body.position, 0);
                npc->stateFrame = 0;
                npc->animationPhase++;
                Sound_PlaySong(0x5B5);
            }
            break;
        case 2: {
            u32 mask = 0x100;
            npc->displayFlags |= mask;
        }
            PlayerParticleCloud_ScatterAnchors();
            npc->stateFrame = 0;
            npc->animationPhase++;
            break;
        case 3: {
            u32 mask = 0x100;
            npc->displayFlags |= mask;
        }
            if (tick > 0xF0) {
                void (*callback)(void);
                CpuSet(gPaletteBankData + 0x2AE0, npc->workPalette, 0x04000008);
                PlayerParticleCloud_RequestStop();
                npc->done = 1;
                callback = Actor_OnHiddenIdleUpdate;
                npc->prevState = 1;
                npc->stateCallback = callback;
                npc->stateFrame = 0;
                npc->animationEntered = 1;
                npc->animationPhase = 0;
            }
            break;
    }
}
