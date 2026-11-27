#include "global.h"
struct RecordMenuSprite {
    s16 x, y;
    u8 pad[0x5c];
};
struct RecordMenu {
    u8 pad[0x44];
    struct RecordMenuSprite sprites[9];
};
void RecordMenu_PositionSprites(struct RecordMenu *p)
{
    s32 i;
    for (i = 0; i <= 8; i++) {
        switch (i) {
            case 6:
                p->sprites[i].x = 0;
                p->sprites[i].y = 0;
                break;
            case 7:
                p->sprites[i].x = 0x90;
                p->sprites[i].y = 0;
                break;
            case 8:
                p->sprites[i].x = -8;
                p->sprites[i].y = -0x40;
                break;
            default:
                p->sprites[i].x = 0;
                p->sprites[i].y = 0;
                break;
        }
    }
}
