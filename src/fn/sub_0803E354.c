#include "scene_pool.h"
#include "monster_particle_random.h"
void ScenePool_SetSpeedControlSchedule(struct ScenePoolEntry *, s32, s32, s32);
void ScenePool_SetLateralControlSchedule(struct ScenePoolEntry *, s32, s32, s32);
s32 ScenePool_SelectStateAtPositionLimit(struct ScenePoolEntry *);
void ScenePool_ApplyScheduledSpeedControl(struct ScenePoolActor *, struct ScenePoolEntry *);
void ScenePool_UpdateLateralControlPulse(struct ScenePoolActor *, struct ScenePoolEntry *);
void ScenePool_ScheduleSpeedControl(struct ScenePoolEntry *);
static inline void Or(u32 mask, u32 *flags)
{
    *flags |= mask;
}
static inline s32 RandomByte(const u16 *table)
{
    s32 value;
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    value = table[gRandomIndex];
    return value & 0xff;
}
s32 ScenePool_StateScheduleMotion(void *manager, struct ScenePoolActor *actor,
                                  struct ScenePoolEntry *entry)
{
    u32 *history = entry->history;
    if (entry->stateEntered) {
        entry->stateEntered = 0;
        ScenePool_SetSpeedControlSchedule(entry, 0, 30, 40);
        entry->turnStep = 0x200;
        history[0] = 0;
    }
    if (ScenePool_SelectStateAtPositionLimit(entry))
        return 0;
    Or(0x10, &actor->stateFlags);
    if (!entry->delayA) {
        ScenePool_ScheduleSpeedControl(entry);
        /* The shared sampler (monster_particle_random.h) weights the table register as the ROM
         * does: its statement macro doubles the table's refs (README fact 110). */
        if ((s32)(ParticleRandom_ReadSample(gRandomTable) & 0xff) <= 31) {
            s32 z = actor->position.z >> 12;
            if (z <= 18)
                ScenePool_SetLateralControlSchedule(entry, 1, 3, 6);
            else if (z > 47)
                ScenePool_SetLateralControlSchedule(entry, -1, 3, 6);
            else if (RandomByte(gRandomTable) <= 127)
                ScenePool_SetLateralControlSchedule(entry, 1, 3, 6);
            else
                ScenePool_SetLateralControlSchedule(entry, -1, 3, 6);
        }
    } else
        entry->delayA--;
    ScenePool_ApplyScheduledSpeedControl(actor, entry);
    ScenePool_UpdateLateralControlPulse(actor, entry);
    if (entry->direction > -0x50000 && entry->direction < 0x50000)
        history[0]++;
    else
        history[0] = 0;
    if (history[0] > 9) {
        if (entry->direction > 0x14000 && entry->direction < 0x96000) {
            entry->state = 5;
            entry->stateFrame = 0;
            entry->stateEntered = 1;
            Memory_Clear(entry->history, 0x100);
            return 0;
        }
        if (entry->direction > -0x78000 && entry->direction < -0x14000) {
            entry->state = 6;
            entry->stateFrame = 0;
            entry->stateEntered = 1;
            Memory_Clear(entry->history, 0x100);
            return 0;
        }
        if (entry->direction > -0x14000 && entry->direction < 0x14000) {
            entry->state = 4;
            entry->stateFrame = 0;
            entry->stateEntered = 1;
            Memory_Clear(entry->history, 0x100);
            return 0;
        }
    }
    if (entry->stateFrame > 239) {
        entry->state = 0;
        entry->stateFrame = 0;
        entry->stateEntered = 1;
        Memory_Clear(entry->history, 0x100);
        return 0;
    } else
        entry->stateFrame++;
    return 0;
}
