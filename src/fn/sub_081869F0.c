#define Armor_UpdateWhiteFadeSequence sub_081869F0
#define Armor_TryStartAction sub_0818505C
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

s32 sub_0818505C(struct Armor *, s32);
void Armor_StartSlowWhiteFade(struct Armor *);
void sub_081808B4(struct Armor *);
void sub_081809A8(struct Armor *);
void sub_08181778(struct Armor *);

void sub_081869F0(struct Armor *armor)
{
    switch (armor->sequenceState) {
        case 0:
            if (sub_0818505C(armor, 0x31))
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
                        sub_081808B4(armor);
                        break;
                    case 14:
                    case 15:
                        sub_081809A8(armor);
                        break;
                }
                sub_08181778(armor);
                armor->sequenceState = 3;
            }
            break;
        case 3:
            if (sub_0818505C(armor, 0x32)) {
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
