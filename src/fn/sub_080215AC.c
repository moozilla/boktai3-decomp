#include "global.h"
extern u8 *gGameState;
int Div(int, int);
int SceneActors_SetHealthLimits(u8 *p, int current, int maximum, int extra)
{
    *(u16 *)(p + 0x96) = maximum;
    *(u16 *)(p + 0x94) = current;
    *(u16 *)(p + 0x98) = extra;
    if (*(s16 *)(p + 0x94) > *(s16 *)(p + 0x96))
        *(u16 *)(p + 0x94) = *(u16 *)(p + 0x96);
    {
        int ratio;
        u8 *dst;
        if (*(s16 *)(p + 0x96)) {
            ratio = Div(*(s16 *)(p + 0x94) << 7, *(s16 *)(p + 0x96));
            dst = p + 0xe1;
        } else {
            dst = p + 0xe1;
            ratio = 0;
        }
        *dst = ratio;
    }
    *(int *)(gGameState + 0x61c) = *(s16 *)(p + 0x96);
    *(int *)(gGameState + 0x618) = *(s16 *)(p + 0x94);
    return 0;
}
