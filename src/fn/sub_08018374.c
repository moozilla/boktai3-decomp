#include "global.h"
extern u8 *gUnk_02000090;
int sub_080174B8(void *, int);
#define Weather_Stop sub_08018374
int Weather_Stop(void)
{
    u8 *p = gUnk_02000090;
    int state;
    if (!p)
        return -1;
    state = p[29];
    switch (state) {
        case 1:
        case 2:
        case 3:
            sub_080174B8(p, 4);
            break;
    }
    p[32] = 0;
}
