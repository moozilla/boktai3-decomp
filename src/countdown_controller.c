#include "countdown_controller.h"

s32 Countdown_Tick(struct CountdownTask *task)
{
    s32 done;
    s32 *remaining;
    if (gPlayers[0] && gPlayers[0]->kind == 1) {
        done = 0;
        remaining = &gGameState->unk618;
        if (*remaining > 0) {
            if (++task->frame >= task->interval) {
                task->frame = 0;
                --*remaining;
                Hud_UpdateCountdownDisplay(gGameState->unk618, gGameState->unk61C, 1);
                if (gGameState->unk618 <= 0)
                    done = 1;
            }
        } else {
            done = 1;
        }
        if (done) {
            if (gPlayers[0]->tetherMode != 0x18) {
                Player_EnterKindFour(gPlayers[0]);
                {
                    u8 *transition = &gPlayers[0]->tetherMode;
                    s32 zero = 0;
                    *transition = 0x24;
                    gPlayers[0]->tetherPhase = zero;
                    gPlayers[0]->tetherFrame = zero;
                }
                Task_Kill(task);
            }
        }
    }
    return 0;
}

u32 Countdown_Destroy(void)
{
    return 0;
}

s32 Countdown_InitializeFromScript(struct CountdownTask *task, u32 script, u32 context)
{
    s32 *q = &gGameState->unk618;
    if (*q <= 0)
        *q = 1;
    /* Script values are narrowed through the signed halfword representation. */
    *(s16 *)&task->interval = Script_SeekToKeyword('s') ? Script_GetValue() : 30;
    task->frame = 0;
    Hud_UpdateCountdownDisplay(gGameState->unk618, gGameState->unk61C, 0);
    return 0;
}

struct CountdownTask *Countdown_CreateFromScript(u32 script, u32 context)
{
    struct CountdownTask *task = Task_Create(8, sizeof(*task));
    if (task) {
        Task_SetCallbacks(task, (void (*)(void *))Countdown_Tick,
                          (void (*)(void *))Countdown_Destroy);
        if (Countdown_InitializeFromScript(task, script, context) < 0) {
            Task_Kill(task);
            return 0;
        }
    }
    return task;
}
