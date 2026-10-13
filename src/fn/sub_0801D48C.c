#include "global.h"
void ResultTutorial_MovePlayer(void *, int);
int ResultTutorial_RaiseMarker(u8 *p)
{
    switch (p[0xaf]) {
        case 0:
            p[0xaf] = 1;
            *(u16 *)(p + 0xaa) = 0;
        case 1:
            if ((++*(u16 *)(p + 0xaa) & 3) == 0)
                ResultTutorial_MovePlayer(p, 1);
            else
                ResultTutorial_MovePlayer(p, 0);
            if (*(int *)(p + 0x8c) > 4096)
                return 1;
            break;
    }
    return 0;
}
