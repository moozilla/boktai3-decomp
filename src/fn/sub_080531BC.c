#include "stacked_actor.h"
void Sprite_Remove(void *);
void Effect_Remove(struct StackedActorParticle *);
void sub_08020D08(void *);
s32 StackedActor_Destroy(struct StackedActor *actor)
{
    struct StackedActorSprite *segment;
    struct StackedActorParticle *particle;
    s32 i;
    void *trigger;
    Sprite_Remove(&actor->sprite);
    Sprite_Remove(&actor->shadow);
    segment = actor->segments;
    i = 2;
    do {
        Sprite_Remove(segment);
        segment++;
        i--;
    } while (i >= 0);
    trigger = actor->trigger;
    particle = actor->particles;
    i = 7;
    do {
        Effect_Remove(particle);
        particle++;
        i--;
    } while (i >= 0);
    sub_08020D08(trigger);
    if (gUnk_02000118)
        gUnk_02000118 = 0;
    return 0;
}
