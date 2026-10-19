#define InventoryGrid_InitAmountRows sub_08174BBC
#define InventoryGrid_DrawAmount     sub_08174B8C
#include "global.h"
#include "scene_hud_display.h"
void sub_08174B8C(struct SceneHudState *);
void sub_08174BBC(struct SceneHudState *p)
{
    s32 mask = ~1;
    s16 *position = &p->motionWidgets[4].position.x;
    u8 *middle = (void *)&p->motionWidgets[3].flags;
    s32 y = 104;
    u8 *first = (void *)&p->motionWidgets[2].flags;
    s32 i = 2;
    do {
        *(u32 *)first &= mask;
        {
            u16 x = 40;
            *(u16 *)(first + 24) = x;
            *(u16 *)(first + 26) = y;
        }
        *(u32 *)middle &= mask;
        {
            u16 x = 48;
            *(u16 *)(middle + 24) = x;
            *(u16 *)(middle + 26) = y;
        }
        *(u32 *)(middle + 96) &= mask;
        position[0] = 56;
        position[1] = y;
        position += 0x90;
        middle += 288;
        y += 8;
        first += 288;
    } while (--i >= 0);
    sub_08174B8C(p);
}
