#include "damage_number_display.h"

inline static void DamageNumber_ProjectRelativePosition(struct ContactPosition *screen,
                                                        const struct ContactPosition *world,
                                                        const u16 *camera)
{
    s32 x = world->x;
    s32 z = world->z;
    s32 sum;
    s32 vertical;
    s32 y;
    s32 depth;
    s32 scaledSum;
    screen->x = DamageNumber_ProjectScale(3 * (x - z));
    scaledSum = DamageNumber_ProjectScale(3 * (x + z));
    sum = scaledSum;
    vertical = DamageNumber_ProjectScale(3 * world->y);
    y = sum - vertical;
    depth = sum + vertical;
    screen->x = (((u16)screen->x) - camera[0]) + 120;
    screen->y = (y - camera[1]) + 80;
    screen->z = depth - camera[2];
}

struct DamageNumberDisplay *DamageNumberDisplay_SetActiveBit(struct DamageNumberDisplay *display,
                                                             u32 index)
{
    display->activeMask |= 1 << index;
    return display;
}

void DamageNumberDisplay_ClearActiveBit(struct DamageNumberDisplay *display, u32 index)
{
    display->activeMask &= ~(1 << index);
}

u32 DamageNumberDisplay_GetActiveBit(struct DamageNumberDisplay *display, u32 index)
{
    return display->activeMask & (1 << index);
}

s32 DamageNumberDisplay_FindSource(struct DamageNumberDisplay *display, u32 key)
{
    struct DamageNumberGroup *group = display->groups;
    s32 index = 0;
    do {
        if (DamageNumberDisplay_GetActiveBit(display, index) && group->sourceKey == key) {
            return index;
        }
        ++index;
        ++group;
    } while (index <= 3);
    return -1;
}

s32 DamageNumberDisplay_FindAvailableGroup(struct DamageNumberDisplay *display, s32 priority)
{
    struct DamageNumberGroup *group;
    s32 index = 0, result;
    do {
        if (!DamageNumberDisplay_GetActiveBit(display, index)) {
            result = index;
            goto done;
        }
        ++index;
    } while (index <= 3);
    group = display->groups;
    index = 0;
    do {
        if (group->priority <= priority) {
            result = index;
            goto done;
        }
        ++index;
        ++group;
    } while (index <= 3);
    result = -1;
done:
    return result;
}

void SpriteDrawable_SelectTileFrame(void *, void *, u32);

void sub_08217E40(void *, u32);

extern const u32 gUnk_086059C0[];

u32 DamageNumberDisplay_RenderDigits(struct DamageNumberDisplay *display,
                                     struct DamageNumberGroup *group, u32 amount, u32 glyphBase,
                                     struct ContactPosition *initialScreen)
{
    struct ContactPosition screen;
    struct DamageNumberGlyph *digit;
    s32 index;
    s32 value;
    s32 scaledSin;
    const u16 *camera;
    s32 count;
    for (index = 0; index < group->digitCount; index++)
        Effect_Remove(&group->digits[index]);
    group->digitCount = 0;
    group->anchorY = initialScreen->y;
    group->raisedY = initialScreen->y + 6;
    if (amount > 9999) {
        do {
            amount -= 10000;
        } while (amount > 9999);
    }

    count = 0;
    while (amount > 999) {
        ++count;
        amount -= 1000;
    }

    SpriteDrawable_SelectTileFrame(&group->digits[3], display->resource, glyphBase + count);
    if (count > 0)
        group->digitCount = 4;
    count = 0;
    while (amount > 99) {
        ++count;
        amount -= 100;
    }

    SpriteDrawable_SelectTileFrame(&group->digits[2], display->resource, glyphBase + count);
    if ((!group->digitCount) && (count > 0))
        group->digitCount = 3;
    count = 0;
    while (amount > 9) {
        ++count;
        amount -= 10;
    }

    SpriteDrawable_SelectTileFrame(&group->digits[1], display->resource, glyphBase + count);
    if ((!group->digitCount) && (count > 0))
        group->digitCount = 2;
    count = 0;
    while (amount != 0) {
        ++count;
        --amount;
    }

    SpriteDrawable_SelectTileFrame(&group->digits[0], display->resource, glyphBase + count);
    if (!group->digitCount)
        group->digitCount = 1;
    digit = group->digits;
    index = 0;
    if (index < group->digitCount) {
        do {
            s32 angle = (group->phaseFrame << 4) + (index << 5);
            camera = gCameraOrigin;
            if (angle > 128)
                angle = 128;
            DamageNumber_ProjectRelativePosition(&screen, &group->position, camera);
            digit->x = (u16)screen.x - index * 5 - gUnk_086059C0[group->digitCount];
            value = gUnk_086149C4[angle & 255] * 16;
            if (value >= 0)
                scaledSin = value >> 12;
            else
                scaledSin = -((-value) >> 12);
            digit->y = ((u16)screen.y) - scaledSin;
            sub_08217E40(digit, 0);
            ++index;
            ++digit;
        } while (index < group->digitCount);
    }
}

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

void DamageNumberDisplay_RemoveGroup(struct DamageNumberDisplay *display, s32 index)
{
    struct DamageNumberGroup *group = &display->groups[index];
    struct DamageNumberGlyph *digit;
    s32 i = 0;
    if (i < group->digitCount) {
        digit = group->digits;
        do {
            Effect_Remove(digit);
            ++digit;
            ++i;
        } while (i < group->digitCount);
    }
    group->digitCount = 0;
    DamageNumberDisplay_ClearActiveBit(display, index);
}

void DamageNumberGroup_AdvancePhase(struct DamageNumberGroup *group)
{
    ++group->phase;
    group->phaseFrame = 0;
}

s32 DamageNumberDisplay_UpdateBouncing(struct DamageNumberDisplay *display,
                                       struct DamageNumberGroup *group, u32 groupIndex)
{
    struct ContactPosition screen;
    struct DamageNumberGlyph *digit;
    s32 index;
    u32 horizontalOffset;
    s32 value;
    s32 scaledSin;
    DamageNumber_ProjectPosition(&screen, &group->position);
    digit = group->digits;
    index = 0;
    if (index < group->digitCount) {
        struct ContactPosition *projected = &screen;
        const u32 *centers;
        const s16 *sine;
        centers = gUnk_086059C0;
        sine = gUnk_086149C4;
        horizontalOffset = 0;
        do {
            s32 angle = (group->phaseFrame << 4) + (index << 5);
            if (angle > 96)
                angle = 96;
            digit->x = (u16)projected->x - horizontalOffset - centers[group->digitCount];
            value = sine[angle & 255] * 16;
            if (value >= 0)
                scaledSin = value >> 12;
            else
                scaledSin = -((-value) >> 12);
            digit->y = (u16)projected->y - scaledSin;
            horizontalOffset += 5;
            ++index;
            ++digit;
        } while (index < group->digitCount);
    }
    ++group->phaseFrame;
    if (group->phaseFrame > 19)
        DamageNumberGroup_AdvancePhase(group);
    return 0;
}

extern const s16 gUnk_08614A84[];

s32 DamageNumberDisplay_UpdateLeaving(struct DamageNumberDisplay *display,
                                      struct DamageNumberGroup *group, s32 groupIndex)
{
    struct ContactPosition screen;
    struct DamageNumberGlyph *digit;
    s32 index;
    s32 value;
    s32 scaledSin;
    s32 count;
    DamageNumber_ProjectPosition(&screen, &group->position);
    digit = group->digits;
    index = 0;
    count = group->digitCount;
    if (index < count) {
        struct ContactPosition *projected = &screen;
        const u32 *centers;
        const s16 *sine;
        centers = gUnk_086059C0;
        sine = gUnk_08614A84;
        do {
            u32 left = (u16)projected->x - index * 5;
            digit->x = left - centers[count];
            value = *sine * 16;
            if (value >= 0)
                scaledSin = value >> 12;
            else
                scaledSin = -((-value) >> 12);
            digit->y = (u16)projected->y - scaledSin;
            ++index;
            ++digit;
            count = group->digitCount;
        } while (index < count);
    }
    if ((u8)++group->phaseFrame > 9)
        DamageNumberDisplay_RemoveGroup(display, groupIndex);
    return 0;
}

s32 sub_08249244(void *, void *, u32, void *);

extern void (*const gUnk_086059D4[])(struct DamageNumberDisplay *, struct DamageNumberGroup *, u32);

s32 DamageNumberDisplay_Update(struct DamageNumberDisplay *display)
{
    struct DamageNumberGroup *group = display->groups;
    s32 index = 0;
    do {
        if (DamageNumberDisplay_GetActiveBit(display, index))
            sub_08249244(display, group, index, (void *)gUnk_086059D4[group->phase]);
        ++index;
        ++group;
    } while (index <= 3);
    return 0;
}

u32 DamageNumberDisplay_Destroy(struct DamageNumberDisplay *display)
{
    s32 index = 0;
    do {
        DamageNumberDisplay_RemoveGroup(display, index);
        ++index;
    } while (index <= 3);
    return (u32)(gDamageNumberDisplay = NULL);
}

void Effect_Initialize(void *, void *, u32);

void Effect_SetOrigin(void *, s32, s32);

s32 DamageNumberDisplay_Initialize(struct DamageNumberDisplay *display)
{
    struct DamageNumberGroup *group;
    struct DamageNumberGlyph *digit;
    s32 i;
    s32 remaining;
    struct DamageNumberGroup *nextGroup;
    s32 nextIndex;
    gDamageNumberDisplay = display;
    display->resource = AnimData_FindEntry(0x1C1C);
    display->activeMask = 0;
    group = display->groups;
    i = 0;
    do {
        digit = group->digits;
        {
            u32 next = i + 1;
            nextIndex = next;
        }
        {
            u32 offset = sizeof(*group);
            nextGroup = (struct DamageNumberGroup *)(offset + (u32)group);
        }
        remaining = 3;
        do {
            Effect_Initialize(digit, display->resource, 0x10010);
            Effect_SetOrigin(digit, -4, -4);
            SpriteDrawable_SelectTileFrame(digit, display->resource, 0x20);
            digit->priority = 1;
            --remaining;
            ++digit;
        } while (remaining >= 0);
        group->priority = 3;
        {
            u32 zero = 0;
            group->digitCount = zero;
        }
        i = nextIndex;
        group = nextGroup;
    } while (i <= 3);
    return 0;
}

struct DamageNumberDisplay *DamageNumberDisplay_Create(void)
{
    struct DamageNumberDisplay *display;
    if (gDamageNumberDisplay)
        return gDamageNumberDisplay;
    display = Task_Create(9, sizeof(*display));
    if (display) {
        Task_SetCallbacks(display, (void (*)(void *))DamageNumberDisplay_Update,
                          (void (*)(void *))DamageNumberDisplay_Destroy);
        if (DamageNumberDisplay_Initialize(display) < 0) {
            Task_Kill(display);
            return NULL;
        }
    }
    return display;
}

s32 DamageNumberDisplay_Show(struct DamageNumberSource *source, u32 amount, u32 kind,
                             const struct ContactPosition *position,
                             const struct ContactPosition *offset, s32 priority)
{
    struct DamageNumberDisplay *display = gDamageNumberDisplay;
    u32 key;
    s32 index;
    if (!display) {
        display = DamageNumberDisplay_Create();
        if (!display)
            return -1;
    }
    key = ((u32)source << 16) | source->id;
    index = DamageNumberDisplay_FindSource(display, key);
    if (index < 0) {
        index = DamageNumberDisplay_FindAvailableGroup(display, priority);
        if (index < 0)
            return -1;
    } else if (priority < display->groups[index].priority) {
        return -1;
    }
    return (s32)DamageNumberDisplay_SetGroup(display, index, amount, kind, position, offset, key,
                                             priority);
}
