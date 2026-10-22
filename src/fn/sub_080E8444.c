// MATCHING_CONCESSION: register-pinning
#include "global.h"
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
extern struct PlayerFlags *gUnk_02000580[];
extern u32 gUnk_02000560[];
// confidence=low: tests whether the collision target is absent or its player is unavailable.
u8 SceneActor_IsCollisionTargetUnavailable(struct Actor *actor)
{
    struct Target *target = actor->target;
    if (target) {
        if (target->player <= 3) {
            struct PlayerFlags *player = gUnk_02000580[target->player];
            register u32 mask asm("r1") = 0x20000;
            asm("" : "+r"(mask));
            if (player->flags & mask)
                return 1;
            {
                register u32 mask8 asm("r0") = 8;
                u32 flags = gUnk_02000560[target->player];
                asm("" : "+r"(mask8));
                if (flags & mask8)
                    return 1;
            }
        }
        return 0;
    }
    return 1;
}
