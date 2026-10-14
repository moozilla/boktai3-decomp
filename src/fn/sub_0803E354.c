// MATCHING_CONCESSION: register-pinning
#include "scene_pool.h"
#define ScenePool_StateScheduleMotion  sub_0803E354
#define ScenePool_ScheduleSpeedControl sub_0803DAC8
void sub_0803DA30(struct ScenePoolEntry *, s32, s32, s32);
void sub_0803DA7C(struct ScenePoolEntry *, s32, s32, s32);
s32 sub_0803D9D8(struct ScenePoolEntry *);
void sub_0803DBB4(struct ScenePoolActor *, struct ScenePoolEntry *);
void sub_0803DC1C(struct ScenePoolActor *, struct ScenePoolEntry *);
void ScenePool_ScheduleSpeedControl(struct ScenePoolEntry *);
static inline void Or(u32 mask, u32 *flags)
{
    *flags |= mask;
}
s32 ScenePool_StateScheduleMotion(void *manager, struct ScenePoolActor *actor,
                                  struct ScenePoolEntry *entry)
{
    u32 *history = entry->history;
    u32 zero;
    if (entry->stateEntered) {
        u32 initialZero = 0;
        entry->stateEntered = initialZero;
        sub_0803DA30(entry, 0, 30, 40);
        entry->turnStep = 0x200;
        history[0] = initialZero;
    }
    if (sub_0803D9D8(entry))
        goto done;
    Or(0x10, &actor->stateFlags);
    if (!entry->delayA) {
        ScenePool_ScheduleSpeedControl(entry);
        {
            const u16 *table = gUnk_0203B400;
            u32 *index = &gUnk_03005308;
            u32 next;
            u32 indexMask;
            register u32 byteMask asm("r4");
            const u8 *randomAddress;
            s32 random;
            asm("" : "+r"(table), "+r"(index));
            next = *index + 1;
            indexMask = 0x3ff;
            next &= indexMask;
            *index = next;
            randomAddress = (const u8 *)((next << 1) + (u32)table);
            byteMask = 255;
            random = *randomAddress;
            if (random <= 31) {
                s32 z = actor->position[2] >> 12;
                if (z <= 18)
                    sub_0803DA7C(entry, 1, 3, 6);
                else if (z > 47)
                    sub_0803DA7C(entry, -1, 3, 6);
                else {
                    u32 value;
                    u32 second = (next + 1) & indexMask;
                    *index = second;
                    value = table[second];
                    random = value & byteMask;
                    if (random <= 127)
                        sub_0803DA7C(entry, 1, 3, 6);
                    else
                        sub_0803DA7C(entry, -1, 3, 6);
                }
            }
        }
    } else
        entry->delayA--;
    sub_0803DBB4(actor, entry);
    sub_0803DC1C(actor, entry);
    if ((u32)entry->direction + 0x4ffff <= 0x9fffe)
        history[0]++;
    else
        history[0] = 0;
    if (history[0] > 9) {
        register s32 loaded asm("r0") = entry->direction;
        u32 lower = 0xfffebfff;
        u32 window = (u32)loaded + lower;
        u32 upper = 0x81ffe;
        s32 metric = loaded;
        if (window <= upper) {
            entry->state = 5;
            zero = 0;
            goto reset;
        }
        if (metric > -0x78000 && metric < -0x14000) {
            entry->state = 6;
            zero = 0;
            asm("" : "+r"(zero));
            goto reset;
        }
        {
            u32 narrow = (u32)metric + 0x13fff;
            if (narrow <= 0x27ffe) {
                entry->state = 4;
                zero = 0;
                asm("" : "+r"(zero));
                goto reset;
            }
        }
    }
    if (entry->stateFrame <= 239) {
        entry->stateFrame++;
        goto done;
    }
    zero = 0;
    entry->state = zero;
reset:
    entry->stateFrame = zero;
    entry->stateEntered = 1;
    {
        u32 *arg = entry->history;
        Memory_Clear(arg, 0x100);
    }
done:
    return 0;
}
