#include "global.h"
void Armor_InitPartOrbitTimer(void);
void Armor_UpdatePartBobbing(void *);

static inline u8 reset(u8 *p)
{
    s32 one = 1;
    struct F {
        u8 flags;
    } *flag = (struct F *)(p + 0x119);
    u8 old = flag->flags;
    if (old & one) {
        s32 mask = ~1;
        s32 value = mask;
        s32 zero;
        value &= old;
        zero = 0;
        flag->flags = value;
        p[0x12e] = zero;
        return 1;
    }
    return 0;
}
void Armor_StartPartOrbitMotion(u8 *p)
{
    if (reset(p))
        *(void (**)(void))(p + 0x1d0) = Armor_InitPartOrbitTimer;
    Armor_UpdatePartBobbing(p);
}
