
#ifdef NONMATCHING
// fuzzy: 98.4%
// foundry draft (worker best 98.4%); what differs is mostly register allocation and instruction order
#define PursuingActors_ReleaseAll sub_0812C020
#define gPursuingActorManager gUnk_02000184
#include "global.h"
#include "engine.h"
#include "pursuing_actor.h"
void Sprite_Remove(void *);
extern struct PursuingActorManager *gUnk_02000184;
static inline u8 IsActive(struct PursuingActorManager *manager, s32 index)
{
    if (manager->activeSlots & (1 << index)) return 1;
    return 0;
}
u32 sub_0812C020(struct PursuingActorManager *manager)
{
    s32 i, j;
    for (i = 0; i < 8; i++) {
        if (IsActive(manager, i)) {
            struct PursuingActor *actor;
            actor = (void *)(i * sizeof(struct PursuingActor) + 0x3c);
            actor = (void *)((u32)actor + (u32)manager);
            Collider_Remove(&actor->secondary);
            Sprite_Remove(actor);
            for (j = 0; j < 2; j++) {
                if (manager->actors[i].trail[j].active) Sprite_Remove(&actor->trail[j]);
            }
        }
    }
    return (u32)(gUnk_02000184 = 0);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0812C020);
#endif
