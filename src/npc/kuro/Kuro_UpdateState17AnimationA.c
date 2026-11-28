#include "npc/manager/scene_actor_lifecycle.h"
#include "npc/story_character/state_actor.h"

void Kuro_PlayTaggedAnimation(struct SceneActor *, int, int, int, int);
s32 Mod(s32, s32);
void *ActorAttachment_UpdateFromDescriptor(void *, void *, int, int, void *, int, int, int);

void Kuro_UpdateState17AnimationA(void *unused, struct SceneActor *actor, s32 tick)
{
    struct StateActorTag *tag = (struct StateActorTag *)actor->unknown20;
    if (actor->animationEntered) {
        actor->animationEntered = 0;
        actor->finished = 0;
        Kuro_PlayTaggedAnimation(actor, 5, 2, 0, 4);
        actor->drawable.common.resource.keyframes.sprite.frameIndex = 3;
        actor->drawable.common.resource.keyframes.sprite.elapsed = 0;
        Anim_Step(&actor->drawable.common.resource.keyframes.sprite,
                  &actor->drawable.common.resource.keyframes.resource);
    }
    if (tag->field16 && tick > 0) {
        s32 remainder = Mod(tick, 70);
        if (remainder == 0)
            tag->attachment = ActorAttachment_UpdateFromDescriptor(tag->attachment, &actor->body, 2,
                                                                   0, 0, 0, 128, 0);
    }
}
