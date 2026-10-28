// MATCHING_CONCESSION: register-pinning
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
    register struct SceneActorControlSlot *slot asm("r6") = input;
    register struct SceneActorControlNearbyPlayer **location asm("r9") = locationInput;
    u16 args;
    register s8 *index asm("r4") = &player->controlIndex;
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
            flags = (u16 *)((u8 *)flags + 0x16);
            *flags |= 4;
        }
        {
            register struct SceneActorControlNearbyPlayer **read asm("r2") = location;
            u32 offset;
            struct SceneActorControlNearbyPlayer *active = *read;
            offset = 0x4F4;
            slot->parameter = *(u8 *)((u8 *)active + offset);
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
    register struct SceneActorControlSlot *slot asm("r6") = input;
    struct SceneActorControlNearbyPlayer **location = &gPlayers[0];
    register struct SceneActorControlNearbyPlayer *player asm("r5") = *location;
    struct ContactPosition delta;
    struct ContactPosition *q;
    const struct ContactPosition *position;
    if (player) {
        register struct ContactPosition *initial asm("r2") = &delta;
        const struct ContactPosition *source = &slot->savedPosition;
        const struct ContactPosition *playerPosition = &player->position;
        s32 y;
        register u32 dx asm("r12");
        register u32 x asm("r1");
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
        q = initial;
        position = source;
        if ((y < 0 ? -y : y) <= 31 && (u16)(dx + 64) <= 224 && (signedZ = SignedZ(z)) <= 160 &&
            signedZ >= -58 && TryPlayer(slot, player, location))
            return;
        if (TetherActor_CopyPosition(q) && TetherActor_IsStateOne()) {
            register const struct ContactPosition *localSource asm("r2");
            register u32 firstX asm("r0") = (u16)q->x;
            u32 z;
            s32 y;
            s32 signedZ;
            register u32 scratchLifetime asm("r3");
            localSource = position;
            delta.x = firstX - (u16)localSource->x;
            q->y -= localSource->y;
            z = (u16)q->z;
            z -= (u16)localSource->z;
            q->z = z;
            /* This output defines an opaque compiler-only register token. Its
             * value is consumed only by the following empty asm, never by C
             * arithmetic, branches, addresses, or stores. Keep this defining
             * output even if depin can remove it without changing the bytes. */
            y = q->y;
            asm("" : "+r"(y) : "r"(localSource), "r"(scratchLifetime));
            if ((y < 0 ? -y : y) <= 31 && (u16)(*(volatile u16 *)&q->x + 64) <= 224 &&
                (signedZ = SignedZ(z)) <= 160 && signedZ >= -58 &&
                TetherActor_GetParameter() < -1) {
                TetherActor_SetParameter(slot->index);
                {
                    register u16 *flags asm("r2") = &slot->flags;
                    *flags |= 0x40;
                }
                Sound_PlaySong(0x115);
            }
        }
    }
}
