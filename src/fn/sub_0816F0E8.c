#define MenuTransition_UpdateWidgetFlicker sub_0816F0E8
#define gMenuTransitionKeys                gUnk_03005260
#define MenuTransition_SetCallback Hud_SetInteractionCallback
#include "global.h"
void sub_0816D98C(void *);
void sub_0816FF4C(void);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void sub_0816CDB8(void *, int, int);
void sub_0816D920(void *, int);
void sub_0816D868(void *);
void Hud_SaveGridSelection(void *);
void sub_081672FC(void *);
void sub_08166B7C(void *, int);
void sub_0816701C(void *, int, int, int, int, int);
void sub_081663A0(void *, int, int, int, int);
extern u16 gUnk_03005260[];
struct MenuMessageView {
    u8 pad[0x3c];
    void *resource;
    u8 unknown40[0xa4e - 0x40];
    u16 timer;
};
struct MenuFlickerTarget {
    u8 unknown0[0x424];
    u16 x, unknown426, y, z;
};
struct MenuFlickerView {
    u8 unknown0[0xa40];
    struct MenuFlickerTarget *target;
    u8 unknowna44[10];
    u16 timer;
};
s32 sub_08165D10(void *);
void sub_0816D634(void *, u32);
void Hud_DrawDefaultHealthMeter(void *, u32, u32, u32);
void Hud_DrawDefaultEnergyMeter(void *, s32, s32, s32, s32, s32);
void sub_0816F0E8(u8 *p)
{
    u32 r;
    u8 *q, *a;
    if (((struct MenuFlickerView *)p)->timer > 16)
        sub_08165D10(p);
    if ((((struct MenuFlickerView *)p)->timer & 7) <
        7 - (((struct MenuFlickerView *)p)->timer >> 3)) {
        ((struct MenuFlickerView *)p)->target->x = *(u16 *)(p + 0x4868);
        ((struct MenuFlickerView *)p)->target->y = *(u16 *)(p + 0x486c);
    } else {
        ((struct MenuFlickerView *)p)->target->x = *(u16 *)(p + 0x486a);
        ((struct MenuFlickerView *)p)->target->y = *(u16 *)(p + 0x486e);
    }
    Hud_DrawDefaultHealthMeter(p, 1, 5, 3);
    Hud_DrawDefaultEnergyMeter(p, 1, 5, 4, ((struct MenuFlickerView *)p)->target->y,
                 ((struct MenuFlickerView *)p)->target->z);
    {
        u32 next = ((struct MenuFlickerView *)p)->timer + 1;
        r = 0;
        ((struct MenuFlickerView *)p)->timer = next;
    }
    if (((struct MenuFlickerView *)p)->timer > 55) {
        sub_0816D634(p, p[0x438a]);
        sub_0816D98C(p);
        Hud_SetInteractionCallback(p, sub_0816FF4C, 1);
        q = p + 0x4384;
        sub_0816CDB8(q, 1, 0);
        a = p + 0x1620;
        sub_0816D920(a, p[0x4387]);
        sub_0816D868(p);
        Hud_SaveGridSelection(q);
        sub_081672FC(p);
        sub_08166B7C(p, 4);
        sub_0816701C(p, 0, 0, 0, 1, r);
        sub_081663A0(p, 1, 0, 0, r);
    }
}
