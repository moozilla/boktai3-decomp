#include "global.h"
#include "render.h"
struct Weather {
    u8 pad[24];
    u32 timer;
    u8 entered, pad1d[51];
    int bank;
    void *script;
};
extern u8 *gGameState;
void Script_Call(void *, int);
void Weather_EnterPhase(void *, int);
int sub_080177AC(void *, int, int, int);
static inline void setFlag(void)
{
    u32 mask = 4096;
    *(u32 *)(gGameState + 0x868) |= mask;
}
int Weather_StormOnset(struct Weather *p)
{
    int mask;
    if (p->entered) {
        p->entered = 0;
        setFlag();
        if (p->script)
            Script_Call(p->script, 0);
    }
    mask = 512;
    gDispCntShadow = mask | gDispCntShadow;
    if (p->timer <= 14) {
        if ((p->timer & 3) <= 2)
            gDispCntShadow &= ~mask;
    } else if (p->timer <= 29) {
        if ((p->timer & 3) <= 1)
            gDispCntShadow &= ~mask;
    } else {
        if (!(p->timer & 3))
            gDispCntShadow &= ~mask;
    }
    sub_080177AC(p, 3, 10, 10);
    if (p->timer > 44) {
        Weather_EnterPhase(p, 3);
        return 1;
    } else {
        p->bank = 0;
        p->timer++;
        return 0;
    }
}
