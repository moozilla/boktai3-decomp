#include "objects/moving_platforms/named_object.h"

static inline u8 Tst(u32 *m, s32 i)
{
    if (*m & (1 << i))
        return TRUE;
    return FALSE;
}

struct NamedObject *MovingPlatform_AllocateSlot(struct NamedObjectManager *manager)
{
    s32 i;
    struct NamedObject *e;
    for (i = 0; i < 12; i++) {
        if (!Tst(&manager->used, i)) {
            e = &manager->slots[i];
            manager->used |= 1 << i;
            e->index = i;
            return e;
        }
    }
    return 0;
}
