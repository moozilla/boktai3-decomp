#include "global.h"
void ResultTutorial_MovePlayer(void *, int);
int ResultTutorial_LowerMarker(u8 *p)
{
    switch (p[0xaf]) {
        case 0:
            p[0xaf] = 1;
            *(u16 *)(p + 0xaa) = 0;
            break;
        case 1:
            ResultTutorial_MovePlayer(p, 0);
            if (*(int *)(p + 0x8c) <= 3071)
                return 1;
            break;
    }
    return 0;
}
