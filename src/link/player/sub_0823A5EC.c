#include "global.h"
#include "player/player.h"
struct HitSource {
    u8 pad[0xc];
    struct {
        u8 kind;
        u8 pad[3];
    } slots[3];
};
void Player_OnWeaponHitApplyFlame(void);
void Player_OnWeaponHitApplyFrost(void);
void Player_OnWeaponHitApplyCloud(void);
void Player_OnWeaponHitApplyEarth(void);
void Player_RollWeaponHitElementEffect(void);
void Player_RollSecondWeaponHitStatusLow(void);
void Player_RollModifierBonus(void);
void ActorEffect_TestStatusFlags(void);
void Player_GetTargetFlag7Bonus(void);
void Player_GetTargetFlag8Bonus(void);
void Player_GetTargetFlag9Bonus(void);
void Player_GetTargetFlag10Bonus(void);
void Player_GetTargetFlag11Bonus(void);
static inline void ClearFlags(u32 *f, u32 mask)
{
    *f &= ~mask;
}
static inline void SetFlags(u32 *f, u32 mask)
{
    *f |= mask;
}
void sub_0823A5EC(struct Player *p, struct HitSource *src)
{
    s32 i;
    for (i = 0; i < 3; i++) {
        p->flagCallbacks[i] = NULL;
        p->contactCostCallbacks[i] = NULL;
    }
    for (i = 0; i <= 2; i++) {
        switch (src->slots[i].kind) {
            case 2:
                break;
            case 36:
                p->contactCostCallbacks[i] = ActorEffect_TestStatusFlags;
                break;
            case 35:
                p->contactCostCallbacks[i] = Player_RollModifierBonus;
                break;
            case 37:
                p->contactCostCallbacks[i] = Player_GetTargetFlag7Bonus;
                break;
            case 38:
                p->contactCostCallbacks[i] = Player_GetTargetFlag8Bonus;
                break;
            case 39:
                p->contactCostCallbacks[i] = Player_GetTargetFlag9Bonus;
                break;
            case 40:
                p->contactCostCallbacks[i] = Player_GetTargetFlag10Bonus;
                break;
            case 41:
                p->contactCostCallbacks[i] = Player_GetTargetFlag11Bonus;
                break;
            case 5:
                p->flagCallbacks[i] = Player_OnWeaponHitApplyFlame;
                break;
            case 6:
                p->flagCallbacks[i] = Player_OnWeaponHitApplyFrost;
                break;
            case 7:
                p->flagCallbacks[i] = Player_OnWeaponHitApplyCloud;
                break;
            case 8:
                p->flagCallbacks[i] = Player_OnWeaponHitApplyEarth;
                break;
            case 42:
                p->flagCallbacks[i] = Player_RollWeaponHitElementEffect;
                break;
            case 11:
                p->flagCallbacks[i] = Player_RollSecondWeaponHitStatusLow;
                break;
            case 44:
                if (p)
                    ClearFlags(&p->statBonusFlags, 1);
                break;
            case 43:
                if (p)
                    ClearFlags(&p->statBonusFlags, 2);
                break;
            case 13:
            case 14:
                if (p)
                    SetFlags(&p->statBonusFlags, 4);
                break;
        }
    }
}
