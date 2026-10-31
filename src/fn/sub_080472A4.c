#include "damage_number_display.h"
struct DamageNumberDisplay *DamageNumberDisplay_Create(void);
s32 DamageNumberDisplay_FindSource(struct DamageNumberDisplay *, u32);
s32 DamageNumberDisplay_FindAvailableGroup(struct DamageNumberDisplay *, s32);
struct DamageNumberDisplay *DamageNumberDisplay_SetGroup(struct DamageNumberDisplay *, s32, u32,
                                                         u32, const struct ContactPosition *,
                                                         const struct ContactPosition *, u32, u32);
s32 DamageNumberDisplay_Show(struct DamageNumberSource *source, u32 amount, u32 kind,
                             const struct ContactPosition *position,
                             const struct ContactPosition *offset, s32 priority)
{
    struct DamageNumberDisplay *display = gDamageNumberDisplay;
    u32 key;
    s32 index, result;
    if (!display) {
        display = DamageNumberDisplay_Create();
        if (!display)
            goto failed;
    }
    key = ((u32)source << 16) | source->id;
    index = DamageNumberDisplay_FindSource(display, key);
    if (index >= 0)
        goto found;
    index = DamageNumberDisplay_FindAvailableGroup(display, priority);
    if (index >= 0)
        goto setup;
failed:
    result = -1;
    goto done;
found:
    if (priority < display->groups[index].priority)
        goto failed;
setup:
    result = (s32)DamageNumberDisplay_SetGroup(display, index, amount, kind, position, offset, key,
                                               priority);
done:
    return result;
}
