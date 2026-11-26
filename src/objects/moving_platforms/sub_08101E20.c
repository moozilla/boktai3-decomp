#include "objects/moving_platforms/named_object.h"

static inline u8 Tst(u16 *m, s32 one)
{
    if (*m & one)
        return TRUE;
    return FALSE;
}

u32 MovingPlatform_IsMapCellOccupied(u32 cell)
{
    struct NamedObjectManager *manager = gNamedActorManager;
    s32 i;
    for (i = 0; i < 12; i++) {
        if (Tst((u16 *)&manager->slots[i].flags, 1)) {
            if (manager->slots[i].registration.index == cell)
                return 1;
        }
    }
    return 0;
}
