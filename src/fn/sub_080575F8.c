#include "countdown_timer.h"
#define CountdownTimer_CreateFromScript sub_080575F8
struct CountdownTimer *CountdownTimer_CreateFromScript(void *unused1, void *unused2)
{
    struct CountdownTimer *timer = gUnk_0200049C, *created;
    if (!timer) {
        created = Task_Create(8, sizeof(struct CountdownTimer));
        if (created) {
            Task_SetCallbacks(created, (void (*)(void *))sub_08057574,
                              (void (*)(void *))sub_0805759C);
            if (sub_080575A8(created, unused1, unused2) < 0) {
                Task_Kill(created);
                return 0;
            }
        }
        timer = created;
    }
    return timer;
}
