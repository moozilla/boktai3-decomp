// MATCHING_CONCESSION: register-pinning
#include "scene_pool.h"
void ScenePool_UpdateSpeedPulse(struct ScenePoolActor *actor, struct ScenePoolEntry *entry,
                                const struct ScenePoolSpeedBounds *bounds)
{
    u32 active = entry->speedControlActive;
    s32 metric;
    u32 *pulseFlags;
    register u32 pulseControl asm("r0");
    register u32 pulseMask asm("r1");
    if (active) {
        metric = entry->direction;
        {
            s32 hi = bounds->activeHigh;
            s32 lo = bounds->activeLow;
            s32 inside;
            if (metric > hi)
                goto deactivate;
            inside = 1;
            if (metric < lo)
                inside = 0;
            if (inside)
                goto activeContinue;
        }
    deactivate: {
        entry->speedControlActive = 0;
        entry->speedControlDelay = 30;
        entry->speedControlPulse = 0;
        goto done;
    }
    activeContinue:
        if (!entry->speedControlDelay) {
            if (metric > bounds->high)
                entry->speedControlPulse = 16;
            else if (metric < bounds->low)
                entry->speedControlPulse = -16;
            gRandomIndex = (gRandomIndex + 1) & 0x3ff;
            {
                u32 random = *(const u16 *)((gRandomIndex << 1) + (u32)gUnk_0203B400);
                entry->speedControlDelay = (random & 31) + 60;
            }
        } else
            entry->speedControlDelay--;
        {
            u8 *pulseAddress = (u8 *)&entry->speedControlPulse;
            u32 pulseByte = *pulseAddress;
            s32 pulse = entry->speedControlPulse;
            if (pulse > 0) {
                pulseByte--;
                *pulseAddress = pulseByte;
                {
                    register u32 offset asm("r4") = 0x858;
                    asm("" : : "r"(offset));
                    pulseFlags = (u32 *)((u8 *)actor + offset);
                    pulseControl = *pulseFlags;
                    pulseMask = 0x10;
                    goto applyPulse;
                }
            } else if (pulse < 0) {
                pulseByte++;
                *pulseAddress = pulseByte;
                {
                    u32 offset = 0x858;
                    asm("" : : "r"(offset));
                    pulseFlags = (u32 *)((u8 *)actor + offset);
                    pulseControl = *pulseFlags;
                    pulseMask = 0x20;
                }
            applyPulse:
                *pulseFlags = pulseControl | pulseMask;
            }
        }
    } else {
        s32 hi = bounds->high;
        register s32 lo asm("r4");
        metric = entry->direction;
        lo = bounds->low;
        if (metric > hi)
            goto outside;
        {
            s32 inside = 1;
            if (metric < lo)
                inside = 0;
            if (!inside)
                goto outside;
        }
        {
            entry->speedControlActive = 1;
            entry->speedControlDelay = 30;
            entry->speedControlPulse = active;
            goto done;
        }
    outside:
        if (metric - 1 > bounds->emergencyHigh)
            actor->controlFlags |= 0x300;
        else if (metric < lo)
            actor->controlFlags |= 0x20;
        else
            actor->controlFlags |= 0x110;
    }
done:;
}
