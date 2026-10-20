#define Player_UpdateActionPhase sub_08242EA4
#include "global.h"
#ifdef NONMATCHING
// fuzzy: 91.6%
// draft: charge table read and copy order differ (register choice, instruction order)
#include "player.h"
extern u8 *gUnk_020004B4;
extern const u8 gUnk_08E87B0C[];
void Player_AccumulateActionCharge(struct Player *, u32);
void sub_082420D4(struct Player *);
s32 sub_08242D30(struct Player *);
void sub_0823B728(struct Player *, s32);
void sub_0823B7A4(struct Player *);
void Player_ClampPositionToStoredTerrainCell_0823B938(struct Player *);
void Player_UpdateModifier08Drift(struct Player *);
void Player_SetSecondaryActionFlags(struct Player *, u32);
void sub_08242DCC(struct Player *);
s32 Callback_InvokeActor(void *, void *);
void Player_UpdateActionPhase(struct Player *p)
{
    u32 *src;
    s32 charge;
    s32 idx;
    *(u32 *)((u8 *)p + 0x20) |= 0xA8 << 11;
    switch (p->kind) {
        case 1:
            if (*(u32 *)((u8 *)p + 0x20) & 0x10) {
                s32 slot = p->slot;
                u8 *stored = gUnk_020004B4;
                if (stored == NULL || slot < 0) {
                    charge = 0;
                } else {
                    charge = stored[0x5c + slot];
                }
                Player_AccumulateActionCharge(p, charge);
            }
            sub_082420D4(p);
            idx = sub_08242D30(p);
            sub_0823B728(p, idx);
            Callback_InvokeActor(p, ((void **)gUnk_08E87B0C)[idx]);
            if (p->movementVelocityLocked) {
                u32 hi;
                src = (u32 *)((u8 *)p + 0x490);
                hi = src[1];
                *(u32 *)((u8 *)p + 0x38) = src[0];
                *(u32 *)((u8 *)p + 0x3C) = hi;
            }
            sub_08242DCC(p);
            if (p->cameraOffsetActive != 0) {
                sub_0823B7A4(p);
            }
            Player_ClampPositionToStoredTerrainCell_0823B938(p);
            Player_UpdateModifier08Drift(p);
            break;
        case 2:
            Player_SetSecondaryActionFlags(p, 8);
            Callback_InvokeActor(p, (void *)p->triggerAnimationCallback);
            sub_08242DCC(p);
            break;
        default:
            break;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08242EA4);
#endif
