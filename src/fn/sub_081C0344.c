#include "global.h"
#include "render.h"
struct S {
    u8 f[0x20];
    u32 fl;
    u8 g[0x80];
    u32 a4;
};
u32 TetherActor_GetRequestMode(void);
void TetherActor_PositionRequestIndicator(void *);
void TetherActor_UpdateRequestIndicator(struct S *p)
{
    if (gEnginePauseFlags != 0) {
        p->fl |= 1;
    } else {
        p->a4 = TetherActor_GetRequestMode();
        TetherActor_PositionRequestIndicator(p);
    }
}
