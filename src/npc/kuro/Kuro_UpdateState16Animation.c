#include "npc/manager/scene_actor_lifecycle.h"

void Kuro_PlayTaggedAnimation(struct SceneActor *, int, int, int, int);
s32 DisplayAnimation_TestFinished(struct DisplayAnimation *);

void Kuro_UpdateState16Animation(void *unused, struct SceneActor *actor)
{
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        Kuro_PlayTaggedAnimation(actor, 5, 2, 0, 4);
    }
    if (DisplayAnimation_TestFinished(&actor->drawable.common))
        actor->finished = 1;
}
