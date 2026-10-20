#include "global.h"
#include "player.h"
struct TriggerOpener;
s32 Trigger_TryOpenNearby(struct TriggerOpener *);
u32 Player_GetTetherMode(struct Player *);
s32 sub_08242D30(struct Player *p)
{
    s32 mode = p->tetherMode;
    if (p->inputKeys[1] & 0x200)
        mode = Player_GetTetherMode(p);
    if (p->directionFrame != 0)
        mode = 0x1D;
    else if (mode != 0x1D && mode != 0x17 && mode != 0x16 && mode != 0x1C && mode != 4 &&
             mode != 0xE) {
        if (p->inputKeys[1] & 1) {
            if ((u8)Trigger_TryOpenNearby((struct TriggerOpener *)((u8 *)p + 0x24)))
                mode = 0;
            else
                mode = 4;
        } else if ((s16)p->directionHistory[0] == -1 || (p->inputKeys[0] & 0x100))
            mode = 0;
        else
            mode = 1;
    }
    return mode;
}
