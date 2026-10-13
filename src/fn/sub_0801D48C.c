#include "global.h"
#define ResultTutorial_RaiseMarker sub_0801D48C
void sub_0801D354(void *, int);
int ResultTutorial_RaiseMarker(u8 *p)
{
    switch (p[0xaf]) {
        case 0:
            p[0xaf] = 1;
            *(u16 *)(p + 0xaa) = 0;
        case 1:
            if ((++*(u16 *)(p + 0xaa) & 3) == 0)
                sub_0801D354(p, 1);
            else
                sub_0801D354(p, 0);
            if (*(int *)(p + 0x8c) > 4096)
                return 1;
            break;
    }
    return 0;
}
