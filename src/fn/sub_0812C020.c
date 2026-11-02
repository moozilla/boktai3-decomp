#include "global.h"
#include "engine.h"
#include "pursuing_actor.h"
#include "ram.h"

void Sprite_Remove(void *);
static inline u8 IsActive(struct PursuingActorManager *manager, s32 index)
{
    if (manager->activeSlots & (1 << index))
        return 1;
    return 0;
}
u32 PursuingActors_ReleaseAll(struct PursuingActorManager *manager)
{
    s32 i, j;
    for (i = 0; i < 8; i++) {
        if (IsActive(manager, i)) {
            struct PursuingActor *actor = &manager->actors[i];
            Collider_Remove(&actor->secondary);
            Sprite_Remove(actor);
            for (j = 0; j < 2; j++) {
                if (manager->actors[i].trail[j].active)
                    Sprite_Remove(&manager->actors[i].trail[j]);
            }
        }
    }
    return (u32)(gPursuingActorManager = 0);
}
