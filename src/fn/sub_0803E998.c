#include "scene_pool.h"
void ScenePool_UpdateSpeedPulse(struct ScenePoolActor *, struct ScenePoolEntry *,
                                const struct ScenePoolSpeedBounds *);
extern const struct ScenePoolSpeedBounds gUnk_086058B4[];
s32 Player_GetHorizontalVelocity(s32);
void NativeEffect_CreateFromProfile(struct ScenePoolVec32 *, s32, struct ScenePoolVec32 *, void *,
                                    s32, s32, s32, s32);
static inline void Or(u32 mask, u32 *flags)
{
    *flags |= mask;
}
s32 ScenePool_StateEmitBursts(void *manager, struct ScenePoolActor *actor,
                              struct ScenePoolEntry *entry)
{
    s32 level;
    struct ScenePoolVec32 velocity;
    u8 *kindAddress;
    u32 fire;
    u32 steer;
    if (entry->stateEntered)
        entry->stateEntered = 0;
    ScenePool_UpdateSpeedPulse(actor, entry, gUnk_086058B4);
    fire = 0;
    steer = 0;
    {
        u8 *source = &entry->kind;
        u32 initialKind = *source;
        kindAddress = source;
        switch (initialKind) {
            case 2:
                if (entry->stateFrame == 45 || entry->stateFrame == 90 || entry->stateFrame == 135)
                    fire = 1;
                if (entry->stateFrame > 114)
                    break;
                goto applySteering;
                break;
            case 3:
                if (entry->stateFrame == 60)
                    fire = 1;
                if (entry->stateFrame <= 39)
                    steer = 1;
                break;
        }
    }
    if (steer) {
        s32 lateral, magnitude;
    applySteering:
        lateral = entry->unknown5c;
        magnitude = lateral;
        if (lateral < 0)
            magnitude = -magnitude;
        if (magnitude > 0xa000) {
            if (lateral > 0)
                actor->controlFlags |= 0x80;
            else
                actor->controlFlags |= 0x40;
        }
        Or(0x10, &actor->stateFlags);
    }
    if (fire) {
        switch (*kindAddress) {
            case 2: {
                u32 zero;
                velocity.x = Player_GetHorizontalVelocity(entry->motionPhase) + 1;
                zero = 0;
                velocity.y = zero;
                velocity.z = zero;
                level = actor->restrictionMetric - 0x16000;
                NativeEffect_CreateFromProfile(&actor->position, level, &velocity,
                                               actor->emissionProfile, 5, zero, 3, zero);
                break;
            }
            case 3: {
                struct ScenePoolVec32 *position;
                void *profile;
                u32 zero;
                velocity.x = Player_GetHorizontalVelocity(entry->motionPhase) + 1;
                zero = 0;
                velocity.y = zero;
                velocity.z = zero;
                level = actor->restrictionMetric - 0x8000;
                position = &actor->position;
                profile = actor->emissionProfile;
                NativeEffect_CreateFromProfile(position, level, &velocity, profile, 5, 1, 4, zero);
                velocity.x = Player_GetHorizontalVelocity(entry->motionPhase) + 1;
                velocity.y = zero;
                velocity.z = zero;
                NativeEffect_CreateFromProfile(position, level, &velocity, profile, 5, zero, 4, 1);
                Anim_SetKeyframes((u8 *)actor + 0x660, (u8 *)actor + 0x7e0, 30, 2, zero);
                actor->unknown308 = 60;
                break;
            }
        }
    }
    if (entry->stateFrame > 149) {
        s32 random;
        s32 kind;
        u32 *history;
        u32 clear;
        gRandomIndex = (gRandomIndex + 1) & 0x3ff;
        random = *(const u8 *)(RANDOM_TABLE_ADDR + (gRandomIndex << 1));
        kind = *kindAddress;
        switch (kind) {
            case 0: {
                u8 *entered;
                entry->state = kind;
                entry->stateFrame = kind;
                entered = &entry->stateEntered;
                kind = 1;
                *entered = kind;
                Memory_Clear((u32 *)(entered + 8), 0x100);
                break;
            }
            case 1:
                entry->state = 0;
                entry->stateFrame = 0;
                entry->stateEntered = 1;
                Memory_Clear(entry->history, 0x100);
                break;
            case 2:
                if (random <= 63) {
                    entry->state = 2;
                    clear = 0;
                    entry->stateFrame = clear;
                    entry->stateEntered = 1;
                    history = entry->history;
                } else {
                    entry->state = 1;
                    clear = 0;
                    entry->stateFrame = clear;
                    entry->stateEntered = 1;
                    history = entry->history;
                }
                Memory_Clear(history, 0x100);
                break;
            case 3:
                if (random <= 63) {
                    entry->state = 2;
                    entry->stateFrame = 0;
                    entry->stateEntered = 1;
                    Memory_Clear(entry->history, 0x100);
                    break;
                }
                entry->state = 1;
                entry->stateFrame = 0;
                entry->stateEntered = 1;
                Memory_Clear(entry->history, 0x100);
                return 0;
            default:
                return 0;
        }
        return 0;
    } else {
        entry->stateFrame++;
        return 0;
    }
}
