#include "global.h"
#include "link/lobby/record_menu.h"

void RecordMenu_PositionSprites(struct RecordMenu *menu)
{
    s32 i;

    for (i = 0; i < 9; i++) {
        switch (i) {
            case 6:
                menu->sprites[i].x = 0;
                menu->sprites[i].y = 0;
                break;
            case 7:
                menu->sprites[i].x = 144;
                menu->sprites[i].y = 0;
                break;
            case 8:
                menu->sprites[i].x = -8;
                menu->sprites[i].y = -64;
                break;
            default:
                menu->sprites[i].x = 0;
                menu->sprites[i].y = 0;
                break;
        }
    }
}
