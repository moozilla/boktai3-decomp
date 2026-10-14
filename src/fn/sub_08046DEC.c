#include "damage_number_display.h"
struct DamageNumberDisplay *DamageNumberDisplay_SetActiveBit(struct DamageNumberDisplay *, u32);
u32 DamageNumberDisplay_RenderDigits(struct DamageNumberDisplay *, struct DamageNumberGroup *, u32,
                                     u32, struct ContactPosition *);
struct DamageNumberDisplay *DamageNumberDisplay_SetGroup(struct DamageNumberDisplay *display,
                                                         s32 index, u32 amount, u32 kind,
                                                         const struct ContactPosition *position,
                                                         const struct ContactPosition *offset,
                                                         u32 key, u32 priority)
{
    struct ContactPosition screen;
    struct DamageNumberGroup *group = &display->groups[index];
    u32 glyphBase;
    group->sourceKey = key;
    group->priority = priority;
    group->phaseFrame = 0;
    group->phase = 0;
    group->position = *position;
    group->position.x += (u16)offset->x;
    group->position.y += (u16)offset->y;
    group->position.z += (u16)offset->z;
    DamageNumber_ProjectPosition(&screen, &group->position);
    if (priority == 0 || priority == 2) {
        glyphBase = 42;
        if (!kind)
            glyphBase = 32;
        DamageNumberDisplay_RenderDigits(display, group, amount, glyphBase, &screen);
    } else {
        glyphBase = 62;
        if (!kind)
            glyphBase = 52;
        DamageNumberDisplay_RenderDigits(display, group, amount, glyphBase, &screen);
    }
    return DamageNumberDisplay_SetActiveBit(display, index);
}
