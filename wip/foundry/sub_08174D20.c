#define InventoryGrid_UpdateStatMarkers sub_08174D20
#include "global.h"
#include "scene_input.h"
#include "scene_hud_display.h"
void sub_08174D20(struct SceneHudState *owner)
{
    u8 *p = (u8 *)owner;
    s32 i = 0;
    u8 *base = p + 0x48AC;
    u8 *a = p + 0x1FE0;
    u8 *b = p;
    do {
        struct SceneInputWidget *widget = (void *)(b + 0x3400);
        widget->flags &= ~1;
        *(u16 *)(a + 0x1440) = base[i] - 100;
        *(u16 *)(a + 0x1442) = i * 8;
        a += 96;
        b += 96;
    } while (++i <= 2);
    i = 0;
    {
        u16 *out = (u16 *)(p + 0x3540);
        u16 *bonus = (u16 *)(p + 0x48B6);
        u16 *previous = (u16 *)(p + 0x3420);
        a = p + 0x2100;
        b = p;
        do {
            struct SceneInputWidget *widget = (void *)(b + 0x3520);
            widget->flags &= ~1;
            *out = *bonus + *previous;
            *(u16 *)(a + 0x1442) = i * 8;
            out += 48;
            bonus++;
            previous += 48;
            a += 96;
            b += 96;
        } while (++i <= 2);
    }
    i = 0;
    {
        u16 *out = (u16 *)(p + 0x3660);
        u16 *delta = (u16 *)(p + 0x48B0);
        u16 *previous = (u16 *)(p + 0x3540);
        a = p + 0x2220;
        b = p;
        do {
            struct SceneInputWidget *widget = (void *)(b + 0x3640);
            widget->flags &= ~1;
            *out = *delta + *previous;
            *(u16 *)(a + 0x1442) = i * 8;
            out += 48;
            delta++;
            previous += 48;
            a += 96;
            b += 96;
        } while (++i <= 2);
    }
}
