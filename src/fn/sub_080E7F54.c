#include "global.h"
#include "scene_actor_reset.h"
#include "player.h"

static inline s32 Square(s32 x)
{
    return x * x;
}
u8 SceneActor_FindNearbyActivePlayer(struct SceneActorRegistryResetView *p)
{
    s32 i = 0;
    u32 excluded = 8;
    struct ContactPosition difference;
    struct ContactPosition *delta = &difference;
    struct ContactPosition *position = &p->body.position;
    struct Player **cursor = gPlayers;
    do {
        u32 indexOffset = i << 2;
        struct Player *player = *cursor;
        if (player && !(*(u32 *)((u32)gPlayerSlotStatus + indexOffset) & excluded)) {
            struct SceneVec *source = &player->position;
            s32 x, z;
            s16 new_var;
            delta->x = (u16)player->position.x - (u16)position->x;
            delta->y = (u16)source->y - (u16)position->y;
            delta->z = (u16)source->z - (u16)position->z;
            x = Square(delta->x);
            new_var = delta->z;
            z = Square(new_var);
            if (x + z <= 0x2400) {
                struct Player *nearby = *cursor;
                if (nearby->inputKeys[1] & 1) {
                    struct SceneActorReactionMotion *motion = (void *)&p->tracking.target;
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
