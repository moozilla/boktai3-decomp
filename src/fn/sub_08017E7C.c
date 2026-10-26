#include "global.h"
#include "render.h"
struct Weather {
    u8 pad[24];
    u32 timer;
    u8 entered, pad1d[51];
    int bank;
    void *script0, *script;
};
extern u8 *gUnk_02000710;
extern int gUnk_02000080, gUnk_02000084;
extern u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
void Palette_SetBrightnessMode(int);
void Script_Call(void *, int);
void Weather_EnterPhase(void *, int);
int sub_080177AC(void *, int, int, int);
int Mod(int, int);
static inline void clearFlag(void)
{
    u32 *flags = (void *)(gUnk_02000710 + 0x868);
    u32 mask = ~4096;
    *flags &= mask;
}
int Weather_StormEnd(struct Weather *p)
{
    int mask;
    if (p->entered) {
        p->entered = 0;
        Palette_SetBrightnessMode(3);
        clearFlag();
        if (p->script)
            Script_Call(p->script, 0);
    }
    mask = 512;
    gDispCntShadow = mask | gDispCntShadow;
    if (p->timer <= 9 && !(p->timer & 1))
        gDispCntShadow &= ~mask;
    if (p->timer <= 7)
        sub_080177AC(p, 3, 10, 10);
    if (p->timer > 29) {
        Weather_EnterPhase(p, 0);
        {
            u16 *reg = &gDispCntShadow;
            u32 m;
            m = ~512;
            *reg = m & *reg;
        }
        *(int *)(gUnk_02000710 + 0x630) = gUnk_02000080;
        if (gUnk_02000084 > 0) {
            int r;
            gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
            {
                u32 offset = gUnk_03005308 * 2;
                r = *(u16 *)((u8 *)gUnk_0203B400 + offset);
            }
            *(int *)(gUnk_02000710 + 0x630) += Mod(r, gUnk_02000084);
        }
        return 1;
    } else {
        p->bank = 0;
        p->timer++;
        return 0;
    }
}
