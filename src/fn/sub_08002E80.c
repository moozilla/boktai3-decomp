#include "global.h"
#define Palette_UpdateOscillation sub_08002E80
#include "palette_controller.h"
void Palette_UpdateOscillation(struct PaletteController *p)
{
    struct PaletteWave *w=&p->wave;
    if(w->delay==0) {
        if(++w->timer>=w->period) {
            w->timer=0;
            w->value+=w->step;
            if(w->value<=0) {w->value=0;w->step=-w->step;w->delay=w->lowDelay;}
            else if(w->value>=64) {w->value=64;w->step=-w->step;w->delay=w->highDelay;}
            w->dirty=1;w->phase=0;
        }
    } else w->delay--;
    p->blend=w->value;
}
