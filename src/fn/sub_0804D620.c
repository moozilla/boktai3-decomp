#include "global.h"
struct CountdownTask {
    u8 unknown0[0x18];
    u16 frame, interval;
};
struct CountdownPlayer {
    u8 unknown0[0x1C];
    u32 state;
    u8 unknown20[0x437];
    u8 transition;
    u8 transitionPhase, unknown459;
    u16 transitionFrame;
};
extern struct CountdownPlayer *gUnk_02000580;
extern u8 *gUnk_02000710;
void sub_08178238(s32, s32, u32);
void sub_0815E790(struct CountdownPlayer *);
void Task_Kill(void *);
s32 Countdown_Tick(struct CountdownTask *task)
{
    s32 done;
    s32 *remaining;
    if (gUnk_02000580 && gUnk_02000580->state == 1) {
        done = 0;
        remaining = (s32 *)(gUnk_02000710 + 0x618);
        if (*remaining > 0) {
            if (++task->frame >= task->interval) {
                task->frame = 0;
                --*remaining;
                sub_08178238(*(s32 *)(gUnk_02000710 + 0x618), *(s32 *)(gUnk_02000710 + 0x61C), 1);
                if (*(s32 *)(gUnk_02000710 + 0x618) <= 0)
                    done = 1;
            }
        } else {
            done = 1;
        }
        if (done) {
            if (*((u8 *)gUnk_02000580 + 0x457) != 0x18) {
                sub_0815E790(gUnk_02000580);
                {
                    u8 *transition = &gUnk_02000580->transition;
                    s32 zero = 0;
                    *transition = 0x24;
                    gUnk_02000580->transitionPhase = zero;
                    gUnk_02000580->transitionFrame = zero;
                }
                Task_Kill(task);
            }
        }
    }
    return 0;
}
