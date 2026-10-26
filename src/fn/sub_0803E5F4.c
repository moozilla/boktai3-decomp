// MATCHING_CONCESSION: register-pinning
#include "scene_pool.h"
void ScenePool_UpdateSpeedPulse(struct ScenePoolActor *, struct ScenePoolEntry *,
                                const struct ScenePoolSpeedBounds *);
extern const struct ScenePoolSpeedBounds gUnk_0860588C[];
void Player_GetWorldPosition(s32, struct ScenePoolVec32 *);
s32 Player_GetHorizontalVelocity(s32);
s32 Terrain_GetHeightAtFixedPosition(struct ScenePoolVec32 *);
s32 SceneArc_Create(struct ScenePoolVec32 *, struct ScenePoolVec32 *, s32, s32, s32);
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
s32 ScenePool_StateLaunchArc(void *manager, struct ScenePoolActor *actor,
                             struct ScenePoolEntry *entry)
{
    struct ScenePoolVec32 from, to, path;
    u32 fire;
    if (entry->stateEntered)
        entry->stateEntered = 0;
    ScenePool_UpdateSpeedPulse(actor, entry, gUnk_0860588C);
    if (entry->stateFrame <= 29) {
        s32 lateral = entry->unknown5c;
        s32 magnitude = lateral;
        if (lateral < 0)
            magnitude = -magnitude;
        if (magnitude > 0xa000) {
            if (lateral > 0)
                actor->controlFlags |= 0x80;
            else
                actor->controlFlags |= 0x40;
        }
    }
    fire = 0;
    switch (entry->mode) {
        case 0:
            if (entry->stateFrame != 45)
                break;
            goto launch;
            break;
        case 1:
            if (entry->stateFrame == 45 || entry->stateFrame == 135)
                fire = 1;
            break;
    }
    if (fire) {
        s32 speed;
    launch:
        Player_GetWorldPosition(entry->motionPhase, &path);
        from = entry->position;
        from.y += actor->restrictionMetric;
        to = entry->position;
        speed = Player_GetHorizontalVelocity(entry->motionPhase);
        if (speed <= 0xfff)
            speed = 0x1000;
        to.x = path.x + speed * 90;
        to.y = Terrain_GetHeightAtFixedPosition(&to);
        to.z = path.z;
        SceneArc_Create(&from, &to, 0x40, 90, 0x40);
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
            register const u8 *table asm("r3");
            table = (const u8 *)gUnk_0203B400;
            random = *(const u8 *)(offset + (u32)table);
        }
        kind = entry->kind;
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
