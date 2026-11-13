#include "player_3.h"
#include "ram.h"
#include "scene_consumable_definition.h"

struct EquipmentFlagState {
    u8 unknown0[6];
    u16 bonus[3];
    u32 special;
};
void *GameState_GetConsumableDescriptor(u32);
s32 Inventory_GetConsumableId(u32);
s32 GameState_HasEquippedConsumable(u32);
void Player_AddResistance(struct Player *, s32, s32);
void Player_SubtractResistance(struct Player *, s32, s32);
void Player_InitVisualResources(void *);
void Collider_SetContactMode(void *, u32);
static inline void ClearEquipmentFlag(struct Player *p, u32 index)
{
    if (!p)
        return;
    if (index > 63)
        p->actionStateFlags &= ~(1 << (index - 64));
    else if (index > 31)
        p->actionFlags &= ~(1 << (index - 32));
    else
        p->statBonusFlags &= ~(1 << index);
}
static inline void SetEquipmentFlag(struct Player *p, u32 index)
{
    if (!p)
        return;
    if (index > 63)
        p->actionStateFlags |= 1 << (index - 64);
    else if (index > 31)
        p->actionFlags |= 1 << (index - 32);
    else
        p->statBonusFlags |= 1 << index;
}
static inline void ReverseResistance(struct Player *p, u32 index)
{
    s32 value = p->cueDefenses[index] - 4;
    if (value < 0)
        value = -value;
    p->cueDefenses[index] = value + 4;
}
static inline u32 EquipmentSlot(struct SceneValueConsumableDefinition *current)
{
    register u32 slot = current->slot;
    return slot;
}
void PlayerAction_RefreshFlagState(struct Player *p, struct EquipmentFlagState *state,
                                   s32 alternate)
{
    s32 i;
    struct SceneValueConsumableDefinition *override;
    s32 choice;
    state->special = 0;
    {
        u16 zero = 0;
        for (i = 2; i >= 0; i--)
            state->bonus[i] = zero;
    }
    if (p) {
        s32 bit;
        for (bit = 7; bit <= 67; bit++)
            ClearEquipmentFlag(p, bit);
    }
    if (p) {
        if (p->actionMode == 6 || p->slot)
            goto finish;
        choice = alternate;
    } else
        choice = alternate;
    if (choice < 0)
        override = 0;
    else
        override = GameState_GetConsumableDescriptor(choice);
    for (i = 0; i <= 3; i++) {
        struct SceneValueConsumableDefinition *item;
        s32 effect;
        if (override && i == override->slot)
            item = override;
        else {
            struct GameState *save = gGameState;
            const s16 *registered = ((struct GameState *)(i * 2 + (u32)save))->party;

            if (*registered < 0)
                continue;
            item = GameState_GetConsumableDescriptor(Inventory_GetConsumableId(*registered));
        }
        for (effect = 0; effect <= 1; effect++) {
            u32 kind, amount;
            if (!effect) {
                kind = item->effects[0].kind;
                amount = item->effects[0].amount;
            } else {
                kind = item->effects[1].kind;
                amount = item->effects[1].amount;
            }
            switch (kind) {
                case 47:
                case 48:
                case 69:
                case 70:
                case 85:
                case 87:
                case 88:
                case 89:
                    break;
                case 1:
                    state->bonus[1] += amount;
                    break;
                case 2:
                    state->bonus[2] += amount;
                    break;
                case 3:
                    state->bonus[2] += amount;
                    state->bonus[0] += amount;
                    break;
                case 4:
                case 5:
                case 6:
                    state->bonus[0] += amount;
                    break;
                case 11:
                    SetEquipmentFlag(p, 50);
                    break;
                case 7:
                    SetEquipmentFlag(p, 51);
                    break;
                case 8:
                    SetEquipmentFlag(p, 52);
                    break;
                case 9:
                    SetEquipmentFlag(p, 53);
                    break;
                case 10:
                    SetEquipmentFlag(p, 54);
                    break;
                case 12:
                    SetEquipmentFlag(p, 26);
                    break;
                case 13:
                    SetEquipmentFlag(p, 30);
                    break;
                case 14:
                    SetEquipmentFlag(p, 33);
                    break;
                case 15:
                    SetEquipmentFlag(p, 59);
                    break;
                case 16:
                    SetEquipmentFlag(p, 60);
                    break;
                case 17:
                    SetEquipmentFlag(p, 61);
                    break;
                case 18:
                    SetEquipmentFlag(p, 62);
                    SetEquipmentFlag(p, 32);
                    break;
                case 19:
                    SetEquipmentFlag(p, 63);
                    break;
                case 20:
                    SetEquipmentFlag(p, 64);
                    break;
                case 21:
                    SetEquipmentFlag(p, 65);
                    break;
                case 22:
                    Player_AddResistance(p, 2, amount);
                    break;
                case 23:
                    Player_AddResistance(p, 3, amount);
                    break;
                case 24:
                    Player_AddResistance(p, 4, amount);
                    break;
                case 25:
                    Player_AddResistance(p, 5, amount);
                    break;
                case 26:
                    Player_SubtractResistance(p, 5, amount);
                    Player_SubtractResistance(p, 3, amount);
                    Player_SubtractResistance(p, 4, amount);
                    Player_SubtractResistance(p, 2, amount);
                    break;
                case 27:
                    SetEquipmentFlag(p, 18);
                    break;
                case 28:
                    SetEquipmentFlag(p, 15);
                    break;
                case 29:
                    SetEquipmentFlag(p, 17);
                    break;
                case 30:
                    SetEquipmentFlag(p, 12);
                    break;
                case 31:
                    SetEquipmentFlag(p, 9);
                    break;
                case 32:
                    SetEquipmentFlag(p, 45);
                    break;
                case 33:
                    SetEquipmentFlag(p, 44);
                    break;
                case 34:
                    SetEquipmentFlag(p, 8);
                    break;
                case 35:
                    SetEquipmentFlag(p, 29);
                    break;
                case 36:
                    SetEquipmentFlag(p, 11);
                    break;
                case 37:
                    SetEquipmentFlag(p, 34);
                    break;
                case 38:
                    SetEquipmentFlag(p, 35);
                    break;
                case 39:
                    SetEquipmentFlag(p, 36);
                    break;
                case 40:
                    SetEquipmentFlag(p, 37);
                    SetEquipmentFlag(p, 31);
                    break;
                case 41:
                    SetEquipmentFlag(p, 38);
                    break;
                case 42:
                    SetEquipmentFlag(p, 40);
                    break;
                case 43:
                    SetEquipmentFlag(p, 41);
                    break;
                case 44:
                    SetEquipmentFlag(p, 7);
                    break;
                case 45:
                    SetEquipmentFlag(p, 21);
                    SetEquipmentFlag(p, 22);
                    break;
                case 46:
                    SetEquipmentFlag(p, 28);
                    break;
                case 49:
                    state->bonus[2] -= amount;
                    break;
                case 50:
                    SetEquipmentFlag(p, 55);
                    break;
                case 51:
                    SetEquipmentFlag(p, 56);
                    break;
                case 52:
                    SetEquipmentFlag(p, 57);
                    break;
                case 53:
                    SetEquipmentFlag(p, 58);
                    break;
                case 54:
                    SetEquipmentFlag(p, 66);
                    break;
                case 55:
                    SetEquipmentFlag(p, 67);
                    break;
                case 56:
                    Player_SubtractResistance(p, 2, amount);
                    break;
                case 57:
                    Player_SubtractResistance(p, 3, amount);
                    break;
                case 58:
                    Player_SubtractResistance(p, 4, amount);
                    break;
                case 59:
                    Player_SubtractResistance(p, 5, amount);
                    break;
                case 60:
                    SetEquipmentFlag(p, 16);
                    break;
                case 61:
                    SetEquipmentFlag(p, 10);
                    break;
                case 62:
                    SetEquipmentFlag(p, 46);
                    break;
                case 63:
                    SetEquipmentFlag(p, 42);
                    break;
                case 64:
                    SetEquipmentFlag(p, 39);
                    break;
                case 65:
                    SetEquipmentFlag(p, 43);
                    break;
                case 66:
                    SetEquipmentFlag(p, 20);
                    break;
                case 67:
                    SetEquipmentFlag(p, 23);
                    SetEquipmentFlag(p, 24);
                    break;
                case 68:
                    SetEquipmentFlag(p, 49);
                    break;
                case 71:
                    state->bonus[2] += item->effects[0].amount;
                    break;
                case 72:
                    state->special = 1;
                    SetEquipmentFlag(p, 19);
                    break;
                case 73:
                    state->special = 2;
                    break;
                case 74:
                    state->special = 64;
                    break;
                case 75:
                    SetEquipmentFlag(p, 17);
                    break;
                case 76:
                    SetEquipmentFlag(p, 11);
                    break;
                case 77:
                    SetEquipmentFlag(p, 18);
                    break;
                case 78:
                    SetEquipmentFlag(p, 14);
                    break;
                case 79:
                    SetEquipmentFlag(p, 13);
                    break;
                case 80:
                    SetEquipmentFlag(p, 21);
                    break;
                case 81:
                    SetEquipmentFlag(p, 22);
                    break;
                case 82:
                    SetEquipmentFlag(p, 23);
                    break;
                case 83:
                    SetEquipmentFlag(p, 24);
                    break;
                case 84:
                    SetEquipmentFlag(p, 25);
                    break;
                case 86:
                    SetEquipmentFlag(p, 27);
                    break;
            }
        }
    }
    if (GameState_HasEquippedConsumable(10) && GameState_HasEquippedConsumable(47) &&
        GameState_HasEquippedConsumable(26) && GameState_HasEquippedConsumable(37))
        SetEquipmentFlag(p, 28);
    if (GameState_HasEquippedConsumable(8) && GameState_HasEquippedConsumable(24) &&
        GameState_HasEquippedConsumable(35) && GameState_HasEquippedConsumable(45))
        SetEquipmentFlag(p, 48);
    if (GameState_HasEquippedConsumable(9) && GameState_HasEquippedConsumable(25) &&
        GameState_HasEquippedConsumable(36) && GameState_HasEquippedConsumable(46)) {
        ClearEquipmentFlag(p, 16);
        SetEquipmentFlag(p, 15);
        ReverseResistance(p, 5);
        ReverseResistance(p, 3);
        ReverseResistance(p, 4);
        ReverseResistance(p, 2);
        ClearEquipmentFlag(p, 43);
        SetEquipmentFlag(p, 44);
        ClearEquipmentFlag(p, 42);
        SetEquipmentFlag(p, 41);
        SetEquipmentFlag(p, 47);
    }
    if (p) {
        Collider_SetContactMode(&p->collider, 64);
    finish:
        Player_InitVisualResources(p);
        p->savedSpriteResource = -1;
    }
}

