/* Provisional names and object interpretation; offsets follow static accesses. */
#include "global.h"
struct MessagePanelSprite {
    u8 pad0[0x20];
    u16 x, y;
    u8 pad24[0x60 - 0x24];
};

struct MessagePanel {
    u8 pad0[0x24];
    struct MessagePanelSprite sprites[23];
};

void StageResults_InitCursorPositions(struct MessagePanel *panel)
{
    s32 i;
    for (i = 0; i <= 22; i++) {
        switch (i) {
            case 8:
                panel->sprites[i].x = 0xb0;
                panel->sprites[i].y = 0x18;
                break;
            case 9:
                panel->sprites[i].x = 0xb8;
                panel->sprites[i].y = 0x18;
                break;
            case 10:
                panel->sprites[i].x = 0xc0;
                panel->sprites[i].y = 0x18;
                break;
            case 6:
                panel->sprites[i].x = 0xc8;
                panel->sprites[i].y = 0x18;
                break;
            case 11:
                panel->sprites[i].x = 0xd0;
                panel->sprites[i].y = 0x18;
                break;
            case 12:
                panel->sprites[i].x = 0xd8;
                panel->sprites[i].y = 0x18;
                break;
            case 13:
                panel->sprites[i].x = 0xe0;
                panel->sprites[i].y = 0x18;
                break;
            case 14:
                panel->sprites[i].x = 0xd0;
                panel->sprites[i].y = 0x50;
                break;
            case 15:
                panel->sprites[i].x = 0xd8;
                panel->sprites[i].y = 0x50;
                break;
            case 16:
                panel->sprites[i].x = 0xe0;
                panel->sprites[i].y = 0x50;
                break;
            case 17:
                panel->sprites[i].x = 0xd0;
                panel->sprites[i].y = 0x68;
                break;
            case 18:
                panel->sprites[i].x = 0xd8;
                panel->sprites[i].y = 0x68;
                break;
            case 19:
                panel->sprites[i].x = 0xe0;
                panel->sprites[i].y = 0x68;
                break;
            case 20:
                panel->sprites[i].x = 0xd0;
                panel->sprites[i].y = 0x88;
                break;
            case 21:
                panel->sprites[i].x = 0xd8;
                panel->sprites[i].y = 0x88;
                break;
            case 22:
                panel->sprites[i].x = 0xe0;
                panel->sprites[i].y = 0x88;
                break;
            default:
                panel->sprites[i].x = 0;
                panel->sprites[i].y = 0;
                break;
        }
    }
}
