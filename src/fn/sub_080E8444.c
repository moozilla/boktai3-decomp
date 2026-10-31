#include "global.h"
#include "player.h"
struct Target {
    u8 pad[8];
    u8 player;
};
struct Actor {
    u8 pad[0xDC];
    struct Target *target;
};
struct PlayerFlags {
    u8 pad[0x20];
    u32 flags;
};
// confidence=low: tests whether the collision target is absent or its player is unavailable.
u8 SceneActor_IsCollisionTargetUnavailable(struct Actor *actor)
{
    struct Target *target = actor->target;
    if (target) {
        if (target->player <= 3) {
            struct PlayerFlags *player = gPlayers[target->player];
            u32 mask = 0x20000;
            if (player->flags & mask)
                return 1;
            {
                u32 mask8 = 8;
                u32 flags = gPlayerSlotStatus[target->player];
                if (flags & mask8)
                    return 1;
            }
        }
        return 0;
    }
    return 1;
}
