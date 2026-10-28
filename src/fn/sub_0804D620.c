#include "global.h"
#include "player.h"
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
extern u8 *gGameState;
void Hud_UpdateCountdownDisplay(s32, s32, u32);
void Player_EnterKindFour(struct CountdownPlayer *);
void Task_Kill(void *);
s32 Countdown_Tick(struct CountdownTask *task)
{
    s32 done;
    s32 *remaining;
    if ((*(struct CountdownPlayer **)gPlayers) &&
        (*(struct CountdownPlayer **)gPlayers)->state == 1) {
        done = 0;
        remaining = (s32 *)(gGameState + 0x618);
        if (*remaining > 0) {
            if (++task->frame >= task->interval) {
                task->frame = 0;
                --*remaining;
                Hud_UpdateCountdownDisplay(*(s32 *)(gGameState + 0x618),
                                           *(s32 *)(gGameState + 0x61C), 1);
                if (*(s32 *)(gGameState + 0x618) <= 0)
                    done = 1;
            }
        } else {
            done = 1;
        }
        if (done) {
            if (*((u8 *)(*(struct CountdownPlayer **)gPlayers) + 0x457) != 0x18) {
                Player_EnterKindFour((*(struct CountdownPlayer **)gPlayers));
                {
                    u8 *transition = &(*(struct CountdownPlayer **)gPlayers)->transition;
                    s32 zero = 0;
                    *transition = 0x24;
                    (*(struct CountdownPlayer **)gPlayers)->transitionPhase = zero;
                    (*(struct CountdownPlayer **)gPlayers)->transitionFrame = zero;
                }
                Task_Kill(task);
            }
        }
    }
    return 0;
}
