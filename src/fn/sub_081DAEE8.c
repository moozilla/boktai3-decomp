#include "global.h"
#include "task4e01.h"
struct Task4E01Pair {
    u32 first, second;
};
s32 SceneEntry_SetPosition(s32 i, struct Task4E01Pair *pair)
{
    struct Task4E01 *p = ResourceRegistry_GetResource(0x4e01);
    if (!p || i < 0 || i >= p->count)
        return -1;
    {
        struct Task4E01Entry **slot = &p->entries;
        u32 offset = i * 0x1ac;
        u8 *base = (u8 *)*slot;
        *(struct Task4E01Pair *)(base + offset + 0xa8) = *pair;
    }
    return 0;
}
