#include "global.h"
#include "area_effect_link.h"

struct P2 {
    u32 a, b;
};
struct Link {
    u8 pad[0x5c];
    u8 arr[1];
};
extern void *gUnk_08E87B0C[];
void Player_AccumulateActionCharge(u8 *, s32);
void sub_082420D4(u8 *);
s32 sub_08242D30(u8 *);
void sub_0823B728(u8 *, s32);
void Callback_InvokeActor(u8 *, void *);
void Player_SynchronizeColliderState(u8 *);
void sub_0823B7A4(u8 *);
void Player_ClampPositionToStoredTerrainCell_0823B938(u8 *);
void Player_UpdateModifier08Drift(u8 *);
void Player_SetSecondaryActionFlags(u8 *, s32);

static inline long long LinkGet(s32 i)
{
    u8 v;
    if (((struct Link *)gLinkSession) == 0 || i < 0)
        v = 0;
    else
        v = ((struct Link *)gLinkSession)->arr[i];
    return v;
}

void Player_UpdateActionPhase(u8 *p)
{
    u32 *new_var;
    u32 m = 0x54000;
    u32 f = *(u32 *)(p + 0x20) | m;
    new_var = (u32 *)(p + 0x20);
    *new_var = f;
    switch (*(s32 *)(p + 0x1c)) {
        case 1: {
            s32 r;
            if (f & 0x10) {
                Player_AccumulateActionCharge(p, LinkGet(*(s32 *)(p + 0x18)));
            }
            sub_082420D4(p);
            r = sub_08242D30(p);
            sub_0823B728(p, r);
            Callback_InvokeActor(p, gUnk_08E87B0C[r]);
            if (p[0x48c] != 0) {
                *(struct P2 *)(p + 0x38) = *(struct P2 *)(p + 0x490);
            }
            Player_SynchronizeColliderState(p);
            if (*(s16 *)(p + 0x4b6) != 0)
                sub_0823B7A4(p);
            Player_ClampPositionToStoredTerrainCell_0823B938(p);
            Player_UpdateModifier08Drift(p);
            break;
        }
        case 2:
            Player_SetSecondaryActionFlags(p, 8);
            Callback_InvokeActor(p, *(void **)(p + 0x58c));
            Player_SynchronizeColliderState(p);
            break;
    }
}
