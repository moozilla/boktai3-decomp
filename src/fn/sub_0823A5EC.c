#include "global.h"
#ifdef NONMATCHING
// fuzzy: 89.3%
// draft: size matches; the three flag read-modify-write arms load *flags before materializing the constant (original does constant first)
struct HitSource {
    u8 pad[0xc];
    u8 kind;
    u8 pad2[3];
};
struct P0823A5EC {
    u8 pad0[0x444];
    u32 flags;
    u8 pad448[0xC8C - 0x448];
    void *hitHandlers[3];
    void *bonusHandlers[3];
};
void Player_OnWeaponHitApplyFlame(void);
void Player_OnWeaponHitApplyFrost(void);
void Player_OnWeaponHitApplyCloud(void);
void Player_OnWeaponHitApplyEarth(void);
void Player_OnWeaponHitRollEffect1000(void);
void Player_OnWeaponHitRollEffect80000Low(void);
void Player_RollModifierBonus(void);
void ActorEffect_TestStatusFlags(void);
void Player_GetTargetFlag7Bonus(void);
void Player_GetTargetFlag8Bonus(void);
void Player_GetTargetFlag9Bonus(void);
void Player_GetTargetFlag10Bonus(void);
void Player_GetTargetFlag11Bonus(void);
void sub_0823A5EC(struct P0823A5EC *p, u32 *src)
{
    s32 i;
    void **q;
    u32 zero = 0;
    q = p->hitHandlers;
    for (i = 2; i >= 0; i--) {
        q[0] = zero;
        q[3] = zero;
        q++;
    }
    for (i = 0; i <= 2; i++) {
        switch (((u8 *)(src + i))[0xc]) {
            case 2:
                break;
            case 36:
                p->bonusHandlers[i] = ActorEffect_TestStatusFlags;
                break;
            case 35:
                p->bonusHandlers[i] = Player_RollModifierBonus;
                break;
            case 37:
                p->bonusHandlers[i] = Player_GetTargetFlag7Bonus;
                break;
            case 38:
                p->bonusHandlers[i] = Player_GetTargetFlag8Bonus;
                break;
            case 39:
                p->bonusHandlers[i] = Player_GetTargetFlag9Bonus;
                break;
            case 40:
                p->bonusHandlers[i] = Player_GetTargetFlag10Bonus;
                break;
            case 41:
                p->bonusHandlers[i] = Player_GetTargetFlag11Bonus;
                break;
            case 5:
                p->hitHandlers[i] = Player_OnWeaponHitApplyFlame;
                break;
            case 6:
                p->hitHandlers[i] = Player_OnWeaponHitApplyFrost;
                break;
            case 7:
                p->hitHandlers[i] = Player_OnWeaponHitApplyCloud;
                break;
            case 8:
                p->hitHandlers[i] = Player_OnWeaponHitApplyEarth;
                break;
            case 42:
                p->hitHandlers[i] = Player_OnWeaponHitRollEffect1000;
                break;
            case 11:
                p->hitHandlers[i] = Player_OnWeaponHitRollEffect80000Low;
                break;
            case 44:
                if (p) {
                    u32 *f = &p->flags;
                    *f &= ~1;
                }
                break;
            case 43:
                if (p) {
                    u32 *f = &p->flags;
                    *f &= ~2;
                }
                break;
            case 13:
            case 14:
                if (p) {
                    u32 *f = &p->flags;
                    *f |= 4;
                }
                break;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0823A5EC);
#endif
