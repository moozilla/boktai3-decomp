#include "global.h"
struct Rect16 {
    s16 x, y, z, w;
};
void Menu_SetTextRect(void *, void *);
void Password_UpdateCursorPanel(u8 *p)
{
    struct Rect16 r;
    *(u32 *)(p + 0xa8) |= 1;
    *(u32 *)(p + 0x130) &= ~1;
    switch (*(u32 *)(p + 0x6d8)) {
        case 0:
            *(u32 *)&r = 0;
            *(u32 *)(p + 0xa8) &= ~1;
            *(u32 *)(p + 0x130) |= 1;
            Menu_SetTextRect(p + 0xa0, &r);
            break;
        case 1:
            r.x = 0;
            r.y = 0;
            Menu_SetTextRect(p + 0x128, &r);
            break;
        case 2:
            r.x = 0x30;
            r.y = 0;
            Menu_SetTextRect(p + 0x128, &r);
            break;
        case 3:
            r.x = -0x20;
            r.y = 0x20;
            Menu_SetTextRect(p + 0x128, &r);
            break;
        case 4:
            r.x = 0x10;
            r.y = 0x20;
            Menu_SetTextRect(p + 0x128, &r);
            break;
    }
}
