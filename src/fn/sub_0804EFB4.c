#include "resource_station.h"
// MATCHING_CONCESSION: register-pinning
void Player_BeginResourceStationInteraction(struct ResourceStationPlayer *,
                                            struct ResourceStationPosition *, u32);
void ResourceStation_CheckInteraction(struct ResourceStation *station)
{
    struct ResourceStationPlayer *player = station->player;
    s32 x, z;
    u8 *destination;
    register u32 value asm("r0");
    if (player->position.y != station->triggerPosition.y)
        goto outside;
    x = player->position.x - station->triggerPosition.x;
    if (x < 0)
        x = -x;
    z = player->position.z - station->triggerPosition.z;
    if (z < 0)
        z = -z;
    if (x > 128 || z > 128)
        goto outside;
    {
        u32 playerKind = player->stateTag;
        if (playerKind != 1) {
            register u32 offset asm("r2") = 0x264;
            asm("" : "+r"(offset));
            destination = (u8 *)station + offset;
            goto clear;
        } else {
            register u32 nearbyOffset asm("r5") = 0x264;
            u32 sample;
            u32 blocked;
            asm("" : : "r"(nearbyOffset));
            {
                register u8 *nearby asm("r0") = (u8 *)station + nearbyOffset;
                *nearby = playerKind;
            }
            sample = gUnk_02000534;
            blocked = 0;
            if (sample && gPlayerInteractionMode == 5)
                blocked = 1;
            if (blocked == 0) {
                u32 kind;
                u32 playerOffset = 0x25C;
                player = *(struct ResourceStationPlayer **)((u8 *)station + playerOffset);
                {
                    u32 kindOffset = 0x418;
                    kind = *((u8 *)player + kindOffset);
                    if (kind == 3)
                        goto done;
                    if (kind == 4)
                        goto done;
                    if (kind == 5)
                        goto done;
                    {
                        u32 busyOffset = 0x464;
                        const u16 *input;
                        if (*((u8 *)player + busyOffset))
                            goto done;
                        playerOffset += 0xEC;
                        {
                            const u16 **address = (void *)((u8 *)player + playerOffset);
                            input = *address;
                        }
                        if (!(input[1] & 1))
                            goto done;
                        kindOffset += 0x3F;
                        {
                            u8 *address = (u8 *)player + kindOffset;
                            if (*address != 7)
                                goto done;
                        }
                    }
                    {
                        register u32 triggerOffset asm("r5") = 0x254;
                        struct ResourceStationPosition *trigger =
                            (void *)((u8 *)station + triggerOffset);
                        Player_BeginResourceStationInteraction(player, trigger,
                                                               station->interactionVariant);
                        triggerOffset += 0xE;
                        destination = (u8 *)station + triggerOffset;
                        value = 1;
                        goto store;
                    }
                }
            }
        }
    }
done:
    return;
outside: {
    u32 offset = 0x264;
    asm("" : : "r"(offset));
    destination = (u8 *)station + offset;
}
clear:
    value = 0;
store:
    *destination = value;
}
