#include "scene_effect_schedule.h"

static inline u8 Tst(u32 m, s32 one, s32 i)
{
    if (m & (one << i))
        return TRUE;
    return FALSE;
}

struct SceneEffectScheduleEntry *SceneEffectSchedule_FindById(u32 x)
{
    u8 *b = (u8 *)gSceneEffectSchedule;
    s32 i;
    u32 m;
    s32 one;
    u8 *p;
    if (!b)
        return 0;
    i = 0;
    m = *(u32 *)(b + 0x198);
    p = b;
    one = 1;
    for (; i < 12; i++) {
        if (Tst(m, one, i)) {
            u32 o = i * 0x20;
            if (*(u16 *)(p + 0x2C) == x)
                return (void *)(b + (o + 0x18));
        }
        p += 0x20;
    }
    return 0;
}
