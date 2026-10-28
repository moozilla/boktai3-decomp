// MATCHING_CONCESSION: register-pinning
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
static inline u32 *Prepare(struct ScenePoolEntry *entry, u32 state)
{
    entry->state = state;
    entry->stateFrame = 0;
    entry->stateEntered = 1;
    return entry->history;
}
static inline u32 *PrepareZero(struct ScenePoolEntry *entry, u32 zero, u32 one)
{
    entry->state = zero;
    entry->stateFrame = zero;
    {
        u8 *entered = &entry->stateEntered;
        u32 setOne = one;
        asm("" : : "r"(entered), "r"(setOne));
        *entered = setOne;
        return (u32 *)(entered + 8);
    }
}
static inline u32 *PrepareFirst(struct ScenePoolEntry *entry)
{
    u32 one = 1;
    register u8 *entered asm("r0");
    entry->state = one;
    entry->stateFrame = 0;
    entered = &entry->stateEntered;
    *entered = one;
    return (u32 *)(entered + 8);
}
s32 ScenePool_StateEmitBursts(void *manager, struct ScenePoolActor *actor,
                              struct ScenePoolEntry *entry)
{
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
                register s32 level asm("r5");
                u32 zero;
                velocity.x = Player_GetHorizontalVelocity(entry->motionPhase) + 1;
                zero = 0;
                velocity.y = zero;
                velocity.z = zero;
                {
                    register s32 height asm("r0") = actor->restrictionMetric;
                    level = height - 0x16000;
                }
                NativeEffect_CreateFromProfile(&actor->position, level, &velocity,
                                               actor->emissionProfile, 5, zero, 3, zero);
                break;
            }
            case 3: {
                register s32 level asm("r5");
                register struct ScenePoolVec32 *position asm("r9");
                register void *profile asm("r10");
                u32 zero;
                velocity.x = Player_GetHorizontalVelocity(entry->motionPhase) + 1;
                zero = 0;
                velocity.y = zero;
                velocity.z = zero;
                {
                    register s32 height asm("r0") = actor->restrictionMetric;
                    level = height - 0x8000;
                }
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
        register s32 kind asm("r1");
        u32 *arg;
        u32 *index = &gRandomIndex;
        u32 next = (*index + 1) & 0x3ff;
        *index = next;
        {
            u32 offset = next << 1;
            register const u8 *table asm("r2");
            table = (const u8 *)gRandomTable;
            random = *(const u8 *)(offset + (u32)table);
        }
        kind = *kindAddress;
        switch (kind) {
            case 0:
                arg = PrepareZero(entry, kind, 1);
                goto clear;
            case 1:
                arg = PrepareZero(entry, 0, kind);
                asm("" : "+r"(arg));
                goto clear;
            case 2:
                if (random > 63)
                    goto first;
                entry->state = kind;
                goto second;
            first:
                arg = PrepareFirst(entry);
                asm("" : "+r"(arg));
                goto clear;
            case 3:
                if (random > 63)
                    goto isolatedFirst;
                entry->state = 2;
            second:
                entry->stateFrame = 0;
                entry->stateEntered = 1;
                arg = entry->history;
            clear:
                Memory_Clear(arg, 0x100);
                goto done;
            isolatedFirst:
                arg = PrepareFirst(entry);
                Memory_Clear(arg, 0x100);
                asm volatile("");
                goto done;
        }
    } else
        entry->stateFrame++;
done:
    return 0;
}
