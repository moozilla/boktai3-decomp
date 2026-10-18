#include "global.h"

extern u32 gUnk_03004278;
extern u32 gUnk_03003A00;
extern u16 gUnk_03004270;
extern u16 gUnk_03004274;
void CircleWindow_ResetState(void)
{
    gUnk_03004278 = 0;
    gUnk_03003A00 = 0;
    gUnk_03004270 = 1;
    gUnk_03004274 = 0;
}
