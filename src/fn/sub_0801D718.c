#include "global.h"
void sub_0822B2F8(int);
typedef struct {
    u32 pad[2], flags;
} Graphic;
int ResultTutorial_BlinkPressMarker(u8 *p)
{
    int state = p[0xaf];
    switch (state) {
        case 0:
            p[0xaf] = 1;
            *(u16 *)(p + 0xaa) = 0;
            break;
        case 1: {
            int frame = *(u16 *)(p + 0xaa) & 31;
            int visible;
            if (frame == 0)
                sub_0822B2F8(0x2b0);
            visible = 1;
            if (frame <= 19)
                goto show;
            if (frame <= 29)
                visible = 0;
            if (visible)
            show:
                ((Graphic *)(p + 0x2c4))->flags &= ~1;
            else
                ((Graphic *)(p + 0x2c4))->flags |= state;
            if ((*(s16 *)(p + 0xaa) >> 5) > 2)
                return 1;
            ++*(u16 *)(p + 0xaa);
            break;
        }
    }
    return 0;
}
