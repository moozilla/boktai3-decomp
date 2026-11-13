#include "scene_actor_control.h"
#include "player.h"
struct SceneActorControlNearbyPlayer {
    u8 unknown0[0x30];
    struct ContactPosition position;
    u8 unknown38[0x4F3 - 0x38];
    s8 controlIndex;
    u8 counter;
};
u32 TetherActor_CopyPosition(struct ContactPosition *);
u32 TetherActor_IsStateOne(void);
s32 TetherActor_GetParameter(void);
void TetherActor_SetParameter(s32);
void SceneActorRegistry_SendToCategoryPaletteOffset(u32, u32, u16 *);
static inline s32 SignedZ(u32 value)
{
    s32 shifted = value << 16;
    return shifted >> 16;
}
static inline u8 TryPlayer(struct SceneActorControlSlot *input,
                           struct SceneActorControlNearbyPlayer *player,
                           struct SceneActorControlNearbyPlayer **locationInput)
{
    struct SceneActorControlSlot *slot = input;
    struct SceneActorControlNearbyPlayer **location = locationInput;
    u16 args;
    s8 *index = &player->controlIndex;
    if (*index >= 0)
        return 0;
    else {
        {
            s32 value = slot->index;
            *index = value;
        }
        {
            u16 *flags = &slot->flags;
            *flags |= 0x10;
            flags = &slot->collider.flags;
            *flags |= 4;
        }
        {
            struct SceneActorControlNearbyPlayer **read = location;
            struct SceneActorControlNearbyPlayer *active = *read;
            slot->parameter = active->counter;
        }
        args = 6;
        SceneActorRegistry_SendToCategoryPaletteOffset(2, 1, &args);
        Sound_PlaySong(0x115);
        return 1;
    }
    return 0;
}
void SceneActorControl_TryActivateNearby(struct SceneActorControlSlot *input)
{
    struct SceneActorControlSlot *slot = input;
    struct SceneActorControlNearbyPlayer **location = &gPlayers[0];
    struct SceneActorControlNearbyPlayer *player = *location;
    struct ContactPosition delta;
    struct ContactPosition *q;
    const struct ContactPosition *position;
    if (player) {
        struct ContactPosition *initial = &delta;
        const struct ContactPosition *source = &slot->savedPosition;
        const struct ContactPosition *playerPosition = &player->position;
        s32 y;
        u32 dx;
        u32 x;
        u32 z;
        s32 signedZ;
        x = (u16)player->position.x - (u16)source->x;
        dx = x;
        initial->x = dx;
        initial->y = playerPosition->y - source->y;
        z = (u16)playerPosition->z;
        z -= (u16)source->z;
        initial->z = z;
        y = initial->y;
        if ((y < 0 ? -y : y) <= 31 && (u16)(x + 64) <= 224 && (signedZ = SignedZ(z)) <= 160 &&
            signedZ >= -58 && TryPlayer(slot, player, location))
            return;
        q = &delta;
        position = &slot->savedPosition;
        if (TetherActor_CopyPosition(q) && TetherActor_IsStateOne()) {
            const struct ContactPosition *localSource;
            u32 firstX = (u16)q->x;
            u32 z;
            s32 y;
            s32 signedZ;
            localSource = position;
            delta.x = firstX - (u16)localSource->x;
            q->y -= localSource->y;
            z = (u16)q->z;
            z -= (u16)localSource->z;
            q->z = z;
            y = q->y;
            if ((y < 0 ? -y : y) <= 31 && (u16)((u16)q->x + 64) <= 224 &&
                (signedZ = SignedZ(z)) <= 160 && signedZ >= -58 &&
                TetherActor_GetParameter() < -1) {
                TetherActor_SetParameter(slot->index);
                {
                    u16 *flags = &slot->flags;
                    *flags |= 0x40;
                }
                Sound_PlaySong(0x115);
            }
        }
    }
}
