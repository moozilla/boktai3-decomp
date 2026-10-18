#define SceneNpc_TickStateAndPalette sub_081D25AC
#define SceneNpc_BlendPalette        sub_081D1908
#include "global.h"
#include "state_actor.h"
void sub_081D1908(struct StateActor *, s32, s32);
s32 sub_081D25AC(void *ctx, struct StateActor *p)
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
            sub_081D1908(p, tag->paletteBlendFrames, 60);
        }
    }
    return 0;
}
