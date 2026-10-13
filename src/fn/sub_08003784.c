#include "global.h"
#include "color_fade_controller.h"

void sub_08003734(struct ColorFadeChannel *);
void sub_080036F8(struct ColorFadeChannel *);

void sub_08003784(struct ColorFadeChannel *p)
{
    switch (p->mode) {
        case 1:
        case 3:
            sub_08003734(p);
            break;
        case 2:
        case 4:
            sub_080036F8(p);
            break;
    }
}
