#include "falling_objects.h"

void *sub_0800D828(void *a)
{
    struct FallingObjectManager *p;
    if (gUnk_0200046C != 0)
        return gUnk_0200046C;
    p = Task_Create(8, 0x20);
    if (p != 0) {
        Task_SetCallbacks(p, (void *)FallingObjectManager_Update, (void *)sub_0800D7D4);
        if (sub_0800D800(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
