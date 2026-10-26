#include "countdown_timer.h"
struct CountdownTimer *CountdownTimer_CreateFromScript(void *unused1, void *unused2)
{
    struct CountdownTimer *timer = gCountdownTimer, *created;
    if (!timer) {
        created = Task_Create(8, sizeof(struct CountdownTimer));
        if (created) {
            Task_SetCallbacks(created, (void (*)(void *))CountdownTimer_Update,
                              (void (*)(void *))CountdownTimer_Destroy);
            if (CountdownTimer_InitFromScript(created, unused1, unused2) < 0) {
                Task_Kill(created);
                return 0;
            }
        }
        timer = created;
    }
    return timer;
}
