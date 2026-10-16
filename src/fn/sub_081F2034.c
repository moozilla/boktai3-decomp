#include "global.h"
#include "engine.h"
void *sub_0802140C(u32 slot);
void sub_081F1B14(void *);
void sub_081F1BF8(void *);
s32 sub_081F1C2C(void *, void *);
/* Registry routine 5856: returns the actor in slot 7 of the singleton table
   at 0x020000B8, or creates its task (0x630 bytes). */
void *SingletonSlot7_Create(void *arg)
{
    void *p = sub_0802140C(7);
    if (p)
        return p;
    p = Task_Create(8, 0x630);
    if (p) {
        Task_SetCallbacks(p, sub_081F1B14, sub_081F1BF8);
        if (sub_081F1C2C(p, arg) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
