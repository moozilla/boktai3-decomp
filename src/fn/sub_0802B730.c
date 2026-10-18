#include "global.h"
extern u32 gUnk_02000560;
extern const u32 gUnk_08605034[];
s32 GridAttack_TryStartDamageReaction(void *);
void sub_0824923C(void *, u32);
void GridAttack_UpdateNormalActions(u8 *p)
{
    u32 m = 4;
    if (!(gUnk_02000560 & m)) {
        if (GridAttack_TryStartDamageReaction(p) == 0) {
            sub_0824923C(p, gUnk_08605034[p[0xad]]);
        }
    }
}
