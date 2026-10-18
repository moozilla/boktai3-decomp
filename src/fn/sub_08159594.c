#define PlayerAction_RefreshModifiers sub_08159594
#define PlayerAction_SetResponseValue Player_SetResistance
#define ActionRecord_CopyFromEntry    sub_0822D764
#define PlayerAction_ApplyModifiers   sub_0813E608
#define PlayerAction_RefreshFlagState sub_0813EDDC
#define gActionRecords                gUnk_08E6092C
#include "global.h"
#include "player.h"
struct PlayerActionRecord {
    u32 words[8];
};
extern const struct PlayerActionRecord gUnk_08E6092C[];
void Player_SetResistance(struct Player *, s32, u8);
void sub_0822D764(struct PlayerActionEntry *, struct PlayerActionRecord *);
void sub_0813E608(struct Player *, struct PlayerStatModifierContext *, struct PlayerActionRecord *);
void sub_0813EDDC(struct Player *, void *, s32);
void sub_08159594(struct Player *p)
{
    struct PlayerActionRecord record;
    switch (p->actionMode) {
        case 0:
            Player_SetResistance(p, 0, 5);
            Player_SetResistance(p, 1, 3);
            Player_SetResistance(p, 2, 4);
            Player_SetResistance(p, 3, 4);
            Player_SetResistance(p, 4, 4);
            Player_SetResistance(p, 5, 4);
            Player_SetResistance(p, 6, 4);
            break;
        case 1:
            Player_SetResistance(p, 0, 2);
            Player_SetResistance(p, 1, 10);
            Player_SetResistance(p, 2, 4);
            Player_SetResistance(p, 3, 4);
            Player_SetResistance(p, 4, 4);
            Player_SetResistance(p, 5, 4);
            Player_SetResistance(p, 6, 4);
            break;
        case 2:
            Player_SetResistance(p, 0, 10);
            Player_SetResistance(p, 1, 2);
            Player_SetResistance(p, 2, 4);
            Player_SetResistance(p, 3, 4);
            Player_SetResistance(p, 4, 4);
            Player_SetResistance(p, 5, 4);
            Player_SetResistance(p, 6, 4);
            break;
        case 3:
        case 4:
        case 5:
        default:
            break;
        case 6:
            Player_SetResistance(p, 0, 4);
            Player_SetResistance(p, 1, 4);
            Player_SetResistance(p, 2, 4);
            Player_SetResistance(p, 3, 4);
            Player_SetResistance(p, 4, 4);
            Player_SetResistance(p, 5, 4);
            Player_SetResistance(p, 6, 4);
            break;
    }
    if (p->actionMode != 6)
        sub_0822D764(p->statModifierContext.actionEntry, &record);
    else
        record = gActionRecords[47];
    sub_0813E608(p, &p->statModifierContext, &record);
    sub_0813EDDC(p, p->actionFlagState, -1);
}
