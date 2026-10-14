// MATCHING_CONCESSION: register-pinning
#include "scene_pool.h"
#define ScenePool_StateLaunchShortArc sub_0803E7B4
#ifdef NONMATCHING
// fuzzy: 99.1%
void ScenePool_UpdateSteeringPulse(struct ScenePoolActor *, struct ScenePoolEntry *,
                                   const struct ScenePoolSpeedBounds *);
extern const struct ScenePoolSpeedBounds gUnk_086058A0[];
void sub_08116C4C(s32, struct ScenePoolVec32 *);
s32 sub_08116C20(s32);
s32 sub_0811AE68(struct ScenePoolVec32 *);
s32 sub_0803F4CC(struct ScenePoolVec32 *, struct ScenePoolVec32 *, s32, s32, s32);
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
s32 ScenePool_StateLaunchShortArc(void *manager, struct ScenePoolActor *actor,
                                  struct ScenePoolEntry *entry)
{
    struct ScenePoolVec32 from, to, path;
    register u32 fire asm("r2");
    u8 *kindAddress;
    if (entry->stateEntered)
        entry->stateEntered = 0;
    ScenePool_UpdateSteeringPulse(actor, entry, gUnk_086058A0);
    if (entry->stateFrame <= 19) {
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
    {
        register u8 *source asm("r0") = &entry->kind;
        register u32 initialKind asm("r1") = *source;
        kindAddress = source;
        if (initialKind == 3) {
            if (entry->stateFrame == 10)
                goto launch;
            if (entry->stateFrame != 40)
                goto afterCheck;
            goto launch;
        } else {
            switch (entry->mode) {
                case 0:
                    if (entry->stateFrame != 20)
                        break;
                    goto launch;
                case 1:
                    if (entry->stateFrame == 10 || entry->stateFrame == 40)
                        fire = 1;
                    break;
            }
        }
    }
afterCheck:
    if (fire) {
        s32 speed;
    launch:
        sub_08116C4C(entry->motionPhase, &path);
        from = entry->position;
        from.y += actor->restrictionMetric;
        to = entry->position;
        speed = sub_08116C20(entry->motionPhase);
        if (speed <= 0xfff)
            speed = 0x1000;
        to.x += entry->direction + speed * 48;
        to.y = sub_0811AE68(&to);
        to.z = path.z;
        sub_0803F4CC(&from, &to, 0x20, 48, 0x20);
    }
    if (entry->stateFrame > 59) {
        s32 random;
        register s32 kind asm("r1");
        u32 *arg;
        register u32 *index asm("r0") = &gUnk_03005308;
        u32 next = (*index + 1) & 0x3ff;
        *index = next;
        {
            u32 offset = next << 1;
            register const u8 *table asm("r3");
            table = (const u8 *)gUnk_0203B400;
            random = *(const u8 *)(offset + (u32)table);
        }
        {
            u8 *kindSource = kindAddress;
            u8 **kindRef = &kindSource;
            kind = **kindRef;
        }
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

#else
INCLUDE_ASM("asm/nonmatching", sub_0803E7B4);
#endif
