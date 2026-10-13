#include "global.h"
#include "color_fade_controller.h"

void ColorFade_ApplyCommands(struct ColorFadeController *);
void sub_08003784(struct ColorFadeChannel *);
void sub_08003808(struct ColorFadeController *);
u32 sub_080039BC(struct ColorFadeController *p)
{
    ColorFade_ApplyCommands(p);
    sub_08003784(&p->channels[0]);
    sub_08003784(&p->channels[1]);
    sub_08003808(p);
    return 0;
}
