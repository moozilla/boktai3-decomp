#include "global.h"
struct Weather {
    u8 pad[24];
    u32 timer;
    u8 entered;
};
void sub_08003448(int);
void sub_080174B8(void *, int);
#define Weather_WaitForStorm sub_08017BB8
int Weather_WaitForStorm(struct Weather *p)
{
    if (p->entered) {
        p->entered = 0;
        sub_08003448(1);
    }
    if (p->timer > 99) {
        sub_080174B8(p, 2);
        return 1;
    } else {
        p->timer++;
        return 0;
    }
}
