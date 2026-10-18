#include "damage_number_display.h"
void sub_08217E60(void *, void *, u32);
void sub_08217EC4(void *, s32, s32);
void sub_08217EEC(void *, void *, u32);
s32 DamageNumberDisplay_Initialize(struct DamageNumberDisplay *display)
{
    struct DamageNumberGroup *group;
    struct DamageNumberGlyph *digit;
    s32 i;
    s32 remaining;
    struct DamageNumberGroup *nextGroup;
    s32 nextIndex;
    gUnk_02000074 = display;
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
            sub_08217E60(digit, display->resource, 0x10010);
            sub_08217EC4(digit, -4, -4);
            sub_08217EEC(digit, display->resource, 0x20);
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
