#include "scene/actor/scene_actor_control.h"
#include "player/player.h"
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
        slot->flags |= 0x10;
        slot->collider.flags |= 4;
        slot->parameter = (*location)->counter;
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
    struct SceneActorControlNearbyPlayer **location = gPlayers;
    struct SceneActorControlNearbyPlayer *player = *location;
    struct ContactPosition delta;
    const struct ContactPosition *position;
    struct ContactPosition *q;
    if (player) {
        struct ContactPosition *initial = &delta;
        const struct ContactPosition *source = &slot->savedPosition;
        const struct ContactPosition *playerPosition = &player->position;
        s32 y;
        u32 x;
        u32 z;
        s32 signedZ;
        x = (u16)player->position.x - (u16)source->x;
        initial->x = x;
        initial->y = playerPosition->y - source->y;
        z = (u16)playerPosition->z - (u16)source->z;
        initial->z = z;
        y = initial->y;
        if (abs(y) <= 31 && (u16)(x + 64) <= 224 && (signedZ = (s16)z) <= 160 && signedZ >= -58 &&
            TryPlayer(slot, player, location))
            return;
        q = &delta;
        position = &slot->savedPosition;
        if (TetherActor_CopyPosition(q) && TetherActor_IsStateOne()) {
            u32 z;
            s32 y;
            s32 signedZ;
            delta.x = (u16)q->x - (u16)position->x;
            q->y -= position->y;
            z = (u16)q->z - (u16)position->z;
            q->z = z;
            y = q->y;
            if (abs(y) <= 31 && (u16)((u16)q->x + 64) <= 224 && (signedZ = SignedZ(z)) <= 160 &&
                signedZ >= -58 && TetherActor_GetParameter() < -1) {
                TetherActor_SetParameter(slot->index);
                slot->flags |= 0x40;
                Sound_PlaySong(0x115);
            }
        }
    }
}
