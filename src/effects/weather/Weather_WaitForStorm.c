#include "global.h"
struct Weather {
    u8 pad[24];
    u32 timer;
    u8 entered;
};
void Palette_SetBrightnessMode(int);
void Weather_EnterPhase(void *, int);
int Weather_WaitForStorm(struct Weather *p)
{
    if (p->entered) {
        p->entered = 0;
        Palette_SetBrightnessMode(1);
    }
    if (p->timer > 99) {
        Weather_EnterPhase(p, 2);
        return 1;
    } else {
        p->timer++;
        return 0;
    }
}
