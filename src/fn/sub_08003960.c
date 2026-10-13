#include "global.h"
#define ColorFade_ApplyCommands sub_08003960
struct Channel {
    u32 mode, color, start, level, target;
    u16 time, shift;
};
struct Controller {
    u8 task[0x18];
    struct Channel channels[2];
    u32 count;
    u16 types[8];
    s32 args[8][8];
};
void sub_080038B4(struct Controller *, s32, s32 *);
void sub_0800377C(struct Channel *);
void ColorFade_ApplyCommands(struct Controller *p)
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
                sub_080038B4(p, type, args);
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
