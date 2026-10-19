
#ifdef NONMATCHING
// fuzzy: 91.2%
// foundry draft (worker best 91.2%); what differs is mostly register allocation and instruction order
#define SceneActor_FindNearbyActivePlayer sub_080E7F54
#define gPlayerSlots                      gUnk_02000580
#define gPlayerSlotFlags                  gUnk_02000560
#include "global.h"
#include "scene_actor_reset.h"
#include "player.h"
extern u32 gUnk_02000560[4];
static inline s32 Square(s32 x)
{
    return x * x;
}
u8 sub_080E7F54(struct SceneActorRegistryResetView *p)
{
    s32 i = 0;
    u32 excluded = 8;
    struct ContactPosition difference;
    struct ContactPosition *delta = &difference;
    struct ContactPosition *position = &p->position;
    struct Player **cursor = gUnk_02000580;
    do {
        u32 indexOffset = i << 2;
        struct Player *player = *cursor;
        if (player && !(*(u32 *)((u32)gUnk_02000560 + indexOffset) & excluded)) {
            struct SceneVec *source = &player->position;
            s32 x, z;
            delta->x = (u16)player->position.x - (u16)position->x;
            delta->y = (u16)source->y - (u16)position->y;
            delta->z = (u16)source->z - (u16)position->z;
            x = Square(delta->x);
            z = Square(delta->z);
            if (x + z <= 0x2400) {
                struct Player *nearby = *cursor;
                if (nearby->inputKeys[1] & 1) {
                    struct SceneActorReactionMotion *motion = (void *)&p->fieldDC;
                    motion->fieldDC = (u32)&nearby->positionalEffectGroup;
                    return 1;
                }
            }
        }
        cursor++;
        i++;
    } while (i <= 3);
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080E7F54);
#endif
