#include "global.h"
void ResultGame_ScorePress(void *);
void ResultGame_ShowPressMarker(void *);
void ResultTutorial_Press(u8 *p, int pressed)
{
    int state = (s8)p[0xb0];
    if (state == 0 && pressed) {
        p[0xb0] = 1;
        p[0xb1] = 1;
        ResultGame_ScorePress(p);
        {
            u32 v = *(u16 *)(p + 0x46);
            p[0xc0] = v;
        }
        ResultGame_ShowPressMarker(p);
    }
}
