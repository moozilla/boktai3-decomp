#include "global.h"
struct Q {
    u8 f[0x8e8];
    u32 v;
};
extern struct Q *gParticipantActors[];
u32 Player_GetHorizontalVelocity(s32 i)
{
    if (i <= 1 && gParticipantActors[i] != 0)
        return gParticipantActors[i]->v;
    return 0;
}
