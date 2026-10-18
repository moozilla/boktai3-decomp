#include "global.h"
extern const u32 gUnk_08613DCC[];
void sub_0824923C(void *, u32);
void SingletonSlot7_DispatchPhaseCallback(u8 *p)
{
    sub_0824923C(p, gUnk_08613DCC[p[0xae]]);
}
