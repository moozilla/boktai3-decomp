#include "global.h"
#include "record_menu.h"
#define RecordMenu_PositionSprites sub_0810F00C
void sub_0810F00C(struct RecordMenu *menu)
{
    s32 i = 0;
    s32 offset;
    for (; i <= 8; i++) {
        switch (i) {
            case 6:
                menu->sprites[6].x = 0;
                offset = 0x286;
                goto clearY;
            case 7:
                menu->sprites[7].x = 144;
                offset = 0x2e6;
            clearY:
                *(u16 *)((u8 *)menu + offset) = 0;
                break;
            case 8:
                menu->sprites[8].x = 0xfff8;
                menu->sprites[8].y = 0xffc0;
                break;
            default:
                menu->sprites[i].x = 0;
                menu->sprites[i].y = 0;
                break;
        }
    }
}
