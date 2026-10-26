#include "global.h"
#include "engine.h"
#include "pursuing_actor.h"
#include "ram.h"

struct PackedPosition {
    u32 x : 16, y : 16, z : 16, pad : 16;
};
void PursuingActor_CreateHomingSlash(const struct PackedPosition *, s32, s32, s32, s32, s32, s32,
                                     s32, s32, s32, s32);

static inline s32 OptionalValue(void)
{
    s32 value;
    if (Script_GetPc())
        value = Script_GetValue();
    else
        value = 0;
    return value;
}

/* Request retirement for all chasing slashes. */
void PursuingActor_RetireAll(void)
{
    struct PursuingActorManager *manager = gPursuingActorManager;
    u8 *active;
    s32 i, state, stride;
    if (manager) {
        state = 3;
        active = &manager->actors[0].active;
        stride = sizeof(struct PursuingActor);
        i = 7;
        do {
            if (*active == 1)
                *active = state;
            active += stride;
        } while (--i >= 0);
    }
}

/* Enter the recoil state for all chasing slashes. */
#define PursuingActor_RecoilAll sub_0812C158
void PursuingActor_RecoilAll(void)
{
    struct PursuingActorManager *manager = gPursuingActorManager;
    u8 *active;
    s32 i, state, stride;
    u32 enteredOffset;
    u8 *cursor;
    if (manager) {
        state = 2;
        active = &manager->actors[0].active;
        cursor = (u8 *)manager;
        enteredOffset = (u32) & ((struct PursuingActorManager *)0)->actors[0].entered;
        stride = sizeof(struct PursuingActor);
        i = 7;
        do {
            if (*active == 1) {
                *active = state;
                cursor[enteredOffset] = 1;
            }
            active += stride;
            cursor += stride;
        } while (--i >= 0);
    }
}

/* Script keywords: p position, d heading, s speed, a phase, h homing, P collider. */
#define PursuingActor_CreateHomingSlashFromScript sub_0812C198
void PursuingActor_CreateHomingSlashFromScript(void)
{
    struct PackedPosition position;
    s32 heading, speed, phase;
    s32 turnTimer, turnSpeed, turnDelay;
    s32 attackParam1, attackParam2, attackParam3, attackParam4;
    if (Script_SeekToKeyword('p')) {
        position.x = OptionalValue();
        position.y = OptionalValue();
        position.z = OptionalValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    heading = Script_GetKeywordValue('d', 0);
    speed = Script_GetKeywordValue('s', 20);
    phase = Script_GetKeywordValue('a', 0);
    if (Script_SeekToKeyword('h')) {
        if (Script_GetPc())
            turnTimer = Script_GetValue();
        else
            turnTimer = 0;
        if (Script_GetPc())
            turnSpeed = Script_GetValue();
        else
            turnSpeed = 0;
        if (Script_GetPc())
            turnDelay = Script_GetValue();
        else
            turnDelay = 0;
    } else {
        turnTimer = 0;
        turnSpeed = 0;
        turnDelay = 0;
    }
    if (Script_SeekToKeyword('P')) {
        if (Script_GetPc())
            attackParam1 = Script_GetValue();
        else
            attackParam1 = 0;
        if (Script_GetPc())
            attackParam2 = Script_GetValue();
        else
            attackParam2 = 0;
        if (Script_GetPc())
            attackParam3 = Script_GetValue();
        else
            attackParam3 = 0;
        if (Script_GetPc())
            attackParam4 = Script_GetValue();
        else
            attackParam4 = 0;
    } else {
        attackParam1 = 0;
        attackParam2 = 0;
        attackParam3 = 0;
        attackParam4 = 0;
    }
    PursuingActor_CreateHomingSlash(&position, heading, turnTimer, turnSpeed, turnDelay, speed,
                                    phase, attackParam1, attackParam2, attackParam3, attackParam4);
}
