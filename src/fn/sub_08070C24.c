// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
#include "player.h"
#include "ram.h"
s32 Path_GetPointPositionById(u16 *, u32, u32);
u32 SceneActorRegistry_TestNearestPlayerBounds(struct SceneActorRegistryActor *input,
                                               const struct ContactPosition *inputBounds)
{
    struct SceneActorRegistryActor *actor = input;
    register const struct ContactPosition *bounds asm("r8") = inputBounds;
    s32 nearestDistance = 0x7fffffff;
    s32 nearestIndex = -1;
    /* Only x/y/z participate. The fourth halfword remains unspecified through
  * aggregate copies and is never read as a scalar. */
    struct ContactPosition origin, difference, bestDifference;
    if (actor->category == 10) {
        u32 mask = 0x4000000;
        register u32 offset asm("r3") = 0x2d0;
        register u32 *field asm("r0");
        u32 value;
        asm("" : "+r"(offset) : "r"(mask));
        field = (void *)((u8 *)actor + offset);
        value = *field;
        value &= mask;
        if (value)
            goto success;
        goto failure;
    }
    {
        register u32 firstOffset asm("r1") = 0x458;
        register u32 offset asm("r3");
        u8 *field;
        u32 first;
        asm("" : "+r"(firstOffset));
        field = (u8 *)actor + firstOffset;
        first = *field;
        offset = 0x45a;
        asm("" : "+r"(offset));
        field = (u8 *)actor + offset;
        Path_GetPointPositionById((u16 *)&origin, first, *field);
    }
    {
        s32 index = 0;
        s32 initialCount = gPlayerCount;
        if (index < initialCount) {
            s32 count;
            struct ContactPosition *delta = &difference;
            struct ContactPosition *left = &origin;
            register struct Player **playerSlot asm("r12");
            asm("" : "+r"(delta), "+r"(left));
            count = initialCount;
            {
                register struct Player **initial asm("r0") = gPlayers;
                asm("" : "+r"(initial));
                playerSlot = initial;
            }
            do {
                register struct Player **current asm("r1") = playerSlot;
                register struct Player *player asm("r0");
                player = *current;
                if (player) {
                    const struct SceneVec *right = &player->position;
                    s32 x;
                    s32 z;
                    s32 xSquare;
                    s32 zSquare;
                    s32 distance;
                    delta->x = (u16)player->position.x - (u16)left->x;
                    delta->y = (u16)right->y - (u16)left->y;
                    delta->z = (u16)right->z - (u16)left->z;
                    x = delta->x;
                    xSquare = x * x;
                    z = delta->z;
                    zSquare = z * z;
                    {
                        s32 copy = zSquare;
                        distance = (s32)((u32)xSquare + (u32)copy);
                    }
                    if (nearestDistance > distance) {
                        nearestDistance = distance;
                        nearestIndex = index;
                        bestDifference = difference;
                    }
                }
                playerSlot++;
                index++;
            } while (index < count);
        }
    }
    if (nearestIndex < 0)
        goto failure;
    {
        const struct ContactPosition *check;
        s32 component;
        register s32 limit asm("r0");
        {
            u32 wide = (u32)(u16)bestDifference.x << 16;
            component = (s32)wide >> 16;
        }
        check = bounds;
        limit = check->x;
        if (component > limit || component < -limit)
            goto failure;
        {
            u32 wide = (u32)(u16)bestDifference.z << 16;
            component = (s32)wide >> 16;
        }
        limit = check->z;
        if (component > limit || component < -limit)
            goto failure;
    }
success:
    return 1;
failure:
    return 0;
}
