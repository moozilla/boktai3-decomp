#include "global.h"
#include "color_fade_controller.h"

void ColorFade_ApplyCommand(struct ColorFadeController *, s32, s32 *);
void sub_0800377C(struct ColorFadeChannel *);
void ColorFade_ApplyCommands(struct ColorFadeController *p)
{
    u32 i = 0;
    for (; i < p->count; i++) {
        s32 *args = p->args[i];
        s32 type = p->types[i];
        switch (type) {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
                ColorFade_ApplyCommand(p, type, args);
                break;
            case 6:
                sub_0800377C(&p->channels[0]);
                break;
            case 7:
                sub_0800377C(&p->channels[1]);
                break;
        }
    }
    p->count = 0;
}
