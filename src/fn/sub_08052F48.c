#include "stacked_actor.h"
#define StackedActor_FinishPlayerInteraction sub_08052F48
extern u8 *gUnk_02000580;
void sub_0815C99C(u8 *);
void sub_08056CC0(void);
s32 Script_Call(u32, void *);
void StackedActor_FinishPlayerInteraction(struct StackedActor *actor)
{
    actor->sprite.flags &= ~1;
    if (actor->sprite.position.y < actor->world.y + 22)
        actor->sprite.position.y += 2;
    if (actor->height) {
        actor->height--;
        sub_08052418(actor);
    } else {
        sub_0815C99C(gUnk_02000580);
        sub_08056CC0();
        actor->shadow.flags &= ~1;
        if (actor->removeAfterUse)
            sub_08052628(actor, sub_08052FE4);
        else
            sub_08052628(actor, sub_08052A40);
        if (actor->completedScriptId)
            Script_Call(actor->completedScriptId, 0);
    }
}
