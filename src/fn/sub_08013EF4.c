#include "global.h"
struct P {
    u32 a, b;
};
void *ShadowRegistry_GetTask(void);
u8 *ShadowRegistry_FindEntry(void *, u32);
s32 ShadowRegistry_SetEntryPosition(u32 a, struct P *b)
{
    void *p = ShadowRegistry_GetTask();
    u8 *q;
    if (p != 0) {
        q = ShadowRegistry_FindEntry(p, a);
        if (q != 0) {
            *(struct P *)(q + 4) = *b;
            return 0;
        }
    }
    return -1;
}
