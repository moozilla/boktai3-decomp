#define Player_BeginDirectionalCue sub_08158F04
#define gPlayerCueFlags gUnk_030053F4
#include "global.h"
#include "player.h"
extern u32 gUnk_030053F4;
void sub_0823C0A4(struct Player *);
void sub_0813AFEC(struct Player *, s32, s32);
void sub_0815B404(struct Player *, s32, s32, s32);
s32 sub_08158F04(struct Player *p, s32 mode)
{
    s32 result;
    if (gUnk_030053F4 & 0x800) {
        sub_0823C0A4(p);
        sub_0813AFEC(p, 0x21, 0);
        return 0;
    }
    if (p->tetherMode == 6) {
        result = 1;
        sub_0815B404(p, p->facing, 1, 0);
    } else {
        result = 0;
        sub_0815B404(p, p->facing, mode, 0);
    }
    return result;
}
