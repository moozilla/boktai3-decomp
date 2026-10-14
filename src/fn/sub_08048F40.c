#include "tether_actor.h"
#define TetherActor_UpdatePalette sub_08048F40
#define TetherActor_SelectPalette sub_08048EEC
#define TetherActor_BlendPalette sub_08048E0C
#define TetherActor_UpdateGlow sub_08048D70
void TetherActor_SelectPalette(struct TetherActor *, u32);
void TetherActor_BlendPalette(struct TetherActor *);
void TetherActor_UpdateGlow(struct TetherActor *);
void TetherActor_UpdatePalette(struct TetherActor *actor)
{
    if (actor->kind == 9)
        return;
    if (actor->blendRequested || actor->blendHold) {
        if (actor->paletteBlend <= 63)
            ++actor->paletteBlend;
    } else if (actor->paletteBlend)
        --actor->paletteBlend;
    if (actor->paletteOverrideTimer) {
        if (actor->appliedPalette != 9) {
            if (actor->display.animation.kind == 2)
                actor->appliedPalette = 0;
            else
                actor->appliedPalette = 9;
            TetherActor_SelectPalette(actor, actor->appliedPalette);
        }
        --actor->paletteOverrideTimer;
    } else if (actor->paletteBlend) {
        actor->appliedPalette = 8;
        TetherActor_BlendPalette(actor);
    } else {
        if (actor->appliedPalette != actor->paletteIndex) {
            actor->appliedPalette = actor->paletteIndex;
            TetherActor_SelectPalette(actor, actor->appliedPalette);
        }
        TetherActor_UpdateGlow(actor);
    }
}
