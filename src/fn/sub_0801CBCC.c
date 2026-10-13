#include "global.h"
extern u16 gUnknown_03005260[];
void ResultMenu_SendCommand(void *, int, int, void *);
void ResultGame_ScorePress(void *);
void sub_0801C154(void *);
void ResultGame_HandlePress(u8 *p)
{
    int state = (s8)p[0xb0];
    if (state == 0 && (gUnknown_03005260[1] & 1)) {
        ResultMenu_SendCommand(p, 0x34, 0, 0);
        p[0xb0] = 1;
        p[0xb1] = 1;
        ResultGame_ScorePress(p);
        {
            int position = *(int *)(p + 0x8c) >> 5;
            p[0xc0] = position;
        }
        sub_0801C154(p);
    }
}
