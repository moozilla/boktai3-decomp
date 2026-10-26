#include "damage_number_display.h"
void Effect_Initialize(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
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
