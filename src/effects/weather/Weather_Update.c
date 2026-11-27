#include "global.h"
#include "engine/ram.h"
#include "effects/weather/weather_layout.h"
extern int (*const gUnk_08604C80[])(struct Weather *);
extern u8 *gGameState;
void Weather_UpdateTilemapScroll(void *);
void WeatherParticles_Update(void *);
int Weather_Update(struct Weather *p)
{
    int (*f)(struct Weather *) = gUnk_08604C80[p->state];
    if (f)
        f(p);
    if (p->state > 1)
        Weather_UpdateTilemapScroll(p);
    if (p->state == 3) {
        *(s16 *)(gGameState + 0x87a) = 32767;
        gStormActive = 1;
    } else {
        if (*(s16 *)(gGameState + 0x87a) > 0)
            (*(s16 *)(gGameState + 0x87a))--;
        gStormActive = 0;
    }
    if (gWeatherCollisionTimer > 0)
        gWeatherCollisionTimer--;
    WeatherParticles_Update(p);
    p->frame++;
    return 0;
}
