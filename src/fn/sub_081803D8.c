#include "global.h"
struct BossTask {
    u8 pad0[0xAC];
    u8 fieldAC, fieldAD, fieldAE;
    u8 padAF;
    u8 fieldB0;
    u8 padB1[2];
    u8 fieldB3;
    u8 padB4[0x10];
    u32 fieldC4;
    u8 padC8[0x420];
    u8 field4E8;
    u8 pad4E9;
    u8 step, field4EB;
};
void *SceneActors_GetBySlot(u32 slot);
/* Shared state change of the boss tasks (as in Vanargand_ResetFromScript). */
static inline void setState(struct BossTask *p, u8 state)
{
    {
        s32 k1 = 0;
        p->fieldAC = k1;
    }
    p->fieldAD = 0;
    p->fieldAE = state;
    p->fieldC4 = 0;
    p->fieldB0 = 1;
}

static inline void setStep(struct BossTask *p, u32 step)
{
    p->step = step;
}

/* Registry routine 4EC9: cycles the singleton in slot 2 through steps 0-2
   (+0x4EA), entering state 8, 10 or 3, then clears +0xB3, +0x4EB, +0x4E8. */
void BossSlot2_AdvanceStepFromScript(void)
{
    struct BossTask *task = SceneActors_GetBySlot(2);
    struct BossTask *p = task;
    if (p) {
        if (p->step == 0) {
            setStep(p, 1);
            setState(p, 8);
        } else if (p->step == 1) {
            setStep(p, 2);
            setState(p, 10);
        } else {
            setStep(p, 0);
            setState(p, 3);
        }
        {
            u8 k1 = 0;
            task->fieldB3 = k1;
        }
        p->field4EB = 0;
        p->field4E8 = 0;
    }
}
