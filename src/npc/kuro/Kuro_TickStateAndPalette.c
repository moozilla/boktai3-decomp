#include "global.h"
#include "npc/story_character/state_actor.h"
void Kuro_BlendPalette(struct StateActor *, s32, s32);
s32 Kuro_TickStateAndPalette(void *ctx, struct StateActor *p)
{
    struct StateActorTag *tag = &p->tag;
    u32 frame;
    p->done = p->displayFlags = 0;
    frame = p->stateFrame++;
    if (!tag->field6)
        tag->state.word++;
    p->animationCallback(p);
    if (p->stateCallback) {
        ((void (*)(void *, struct StateActor *, s32))p->stateCallback)(ctx, p, frame);
        if (tag->paletteBlendFrames > 0) {
            tag->paletteBlendFrames--;
            Kuro_BlendPalette(p, tag->paletteBlendFrames, 60);
        }
    }
    return 0;
}
