#include "global.h"
#include "player.h"

struct P457 {
    u8 pad[0x457];
    u8 mode;
};
u32 Player_TestAttachmentFlags(struct P457 *, u32);
u32 TetherActor_TestStatusFlags(u32);

u32 sub_08101094(void)
{
    struct P457 **pp = &gPlayers;
    if (Player_TestAttachmentFlags(*pp, 1)) {
        if (TetherActor_TestStatusFlags(3))
            goto no;
    } else {
        if (!TetherActor_TestStatusFlags(3))
            goto no;
    }
    if ((*pp)->mode == 6)
        return 1;
no:
    return 0;
}
