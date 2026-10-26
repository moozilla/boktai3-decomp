#include "repeated_effect.h"
s32 RepeatedEffect_RequestNearby(struct RepeatedEffectPosition *point)
{
    struct RepeatedEffectManager *manager = gRepeatedEffectManager;
    struct RepeatedEffect *entry;
    struct RepeatedEffectPosition *position;
    u8 *state;
    s32 i, count, limit;
    if (manager)
        goto setup;
    goto none;
found:
    entry->request = 1;
    return 1;
setup:
    i = 0;
    count = manager->count;
    if (i >= count)
        goto none;
    limit = count;
    position = &manager->entries[0].sprite.position;
    state = &manager->entries[0].state;
    entry = manager->entries;
    do {
        if (!*state && position->y >= point->y) {
            s32 x = point->x - position->x;
            s32 z;
            if (x < 0)
                x = -x;
            if (x <= 128) {
                z = point->z - position->z;
                if (z < 0)
                    z = -z;
                if (z <= 128) {
                    s32 squaredX = x;
                    s32 squaredZ = z;
                    squaredX *= x;
                    squaredZ *= z;
                    if (squaredX + squaredZ <= 16384)
                        goto found;
                }
            }
        }
        position = (void *)((u8 *)position + 0xFC);
        state += 0xFC;
        entry++;
        i++;
    } while (i < limit);
none:
    return 0;
}
