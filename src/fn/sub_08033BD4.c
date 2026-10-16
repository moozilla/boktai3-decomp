#include "global.h"
#include "engine.h"
extern void *gUnk_020000E4; /* text window manager */
s32 sub_08033B44(void *);
s32 sub_08033B94(void *);
s32 sub_08033BC0(void *);
/* Script: returns the text window manager task, creating it on first use. */
void *TextWindowManager_Create(void)
{
    void *p = gUnk_020000E4;
    if (!p) {
        p = Task_Create(12, 0x20);
        if (p) {
            Task_SetCallbacks(p, (void (*)(void *))sub_08033B44, (void (*)(void *))sub_08033B94);
            if (sub_08033BC0(p) < 0) {
                Task_Kill(p);
                return 0;
            }
        }
    }
    return p;
}
