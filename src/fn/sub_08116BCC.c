#include "global.h"
extern u8 *gGameState;
struct S {
    u8 f[0x74];
    u32 a;
};
extern struct S *gParticipantActors;
void Player_SaveCurrentHealth(void)
{
    if (gParticipantActors)
        *(u16 *)(gGameState + 0x758) = gParticipantActors->a;
}
