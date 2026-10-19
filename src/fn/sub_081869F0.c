#include "global.h"

struct Armor {
    u8 unknown[0xB38];
    u8 variant;
    u8 unknownB39[4];
    u8 sequenceState;
    u8 sequenceTimer;
    u8 unknownB3F;
    u8 actionState;
};

s32 Armor_TryStartAction(struct Armor *, s32);
void Armor_StartSlowWhiteFade(struct Armor *);
void SceneCombatActor_ResetAttackMode(struct Armor *);
void SceneCombatActor_ResetDefenseMode(struct Armor *);
void Armor_BindPaletteBanks(struct Armor *);

void Armor_UpdateWhiteFadeSequence(struct Armor *armor)
{
    switch (armor->sequenceState) {
        case 0:
            if (Armor_TryStartAction(armor, 0x31))
                armor->sequenceState = 1;
            break;
        case 1:
            if (armor->actionState == 0xFF) {
                Armor_StartSlowWhiteFade(armor);
                armor->sequenceState = 2;
                armor->sequenceTimer = 0;
            }
            break;
        case 2:
            if (armor->sequenceTimer > 15) {
                switch (armor->variant) {
                    case 12:
                    case 13:
                        SceneCombatActor_ResetAttackMode(armor);
                        break;
                    case 14:
                    case 15:
                        SceneCombatActor_ResetDefenseMode(armor);
                        break;
                }
                Armor_BindPaletteBanks(armor);
                armor->sequenceState = 3;
            }
            break;
        case 3:
            if (Armor_TryStartAction(armor, 0x32)) {
                armor->sequenceState = 4;
                armor->sequenceTimer = 0;
            }
            break;
        case 4:
            if (armor->actionState == 0xFF)
                armor->sequenceState = 0xFF;
            break;
    }
}
