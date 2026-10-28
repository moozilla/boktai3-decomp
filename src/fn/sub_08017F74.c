#include "global.h"
#include "ram.h"
struct Weather {
    u8 pad[29];
    u8 state;
    u8 pad1e[62];
    u32 frame;
};
extern int (*const gUnk_08604C80[])(struct Weather *);
extern u8 *gGameState;
extern int gUnk_02000478;
void sub_08017510(void *);
void WeatherParticles_Update(void *);
int Weather_Update(struct Weather *p)
{
    int (*f)(struct Weather *) = gUnk_08604C80[p->state];
    if (f)
        f(p);
    if (p->state > 1)
        sub_08017510(p);
    if (p->state == 3) {
        *(s16 *)(gGameState + 0x87a) = 32767;
        gStormActive = 1;
    } else {
        if (*(s16 *)(gGameState + 0x87a) > 0)
            (*(s16 *)(gGameState + 0x87a))--;
        gStormActive = 0;
    }
    if (gUnk_02000478 > 0)
        gUnk_02000478--;
    WeatherParticles_Update(p);
    p->frame++;
    return 0;
}
