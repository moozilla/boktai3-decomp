#define PlayerAction_SetResponseValue Player_SetResistance
#define gActionRecords                gUnk_08E6092C
#include "global.h"
#include "player.h"
struct PlayerActionRecord {
    u32 words[8];
};
extern const struct PlayerActionRecord gUnk_08E6092C[];
void Player_SetResistance(struct Player *, s32, u8);
void ActionRecord_CopyFromEntry(struct PlayerActionEntry *, struct PlayerActionRecord *);
void Player_BuildActionModifiers(struct Player *, struct PlayerStatModifierContext *, struct PlayerActionRecord *);
void PlayerAction_RefreshFlagState(struct Player *, void *, s32);
void PlayerAction_RefreshModifiers(struct Player *p)
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
        ActionRecord_CopyFromEntry(p->statModifierContext.actionEntry, &record);
    else
        record = gActionRecords[47];
    Player_BuildActionModifiers(p, &p->statModifierContext, &record);
    PlayerAction_RefreshFlagState(p, p->actionFlagState, -1);
}
