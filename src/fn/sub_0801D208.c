#include "global.h"
void ResultGame_ShowTargetMarker(void *);
int ResultTutorial_SlideMarkerIn(u8 *p)
{
    switch (p[0xaf]) {
        case 0:
            ResultGame_ShowTargetMarker(p);
            *(int *)(p + 0x8c) = 0;
            *(int *)(p + 0x9c) = 0;
            ++p[0xaf];
            *(u16 *)(p + 0xaa) = 0;
            break;
        case 1:
            *(int *)(p + 0x8c) += 176;
            *(int *)(p + 0x9c) = *(int *)(p + 0x8c);
            if (*(int *)(p + 0x8c) > 3584) {
                *(int *)(p + 0x8c) = 3584;
                return 1;
            }
            break;
    }
    return 0;
}
