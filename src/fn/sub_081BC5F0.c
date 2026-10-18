#define StatGauge_DrawPreview sub_081BC5F0
#define StatGauge_Draw sub_08243B54
#define gPlayerSlots gUnk_02000580
#define gGameState gUnk_02000710
#include "global.h"
#include "player.h"
#include "game_state.h"
void sub_08243B54(s32, s32, s32, s32, s32, s32);
struct StatGaugePreview {
    u8 unknown0[0x8ee];
    u16 bonus;
    u8 unknown8f0[4];
    u32 exitScript;
};
void sub_081BC5F0(struct StatGaugePreview *p)
{
    s32 current, maximum;
    if (p->exitScript == 0) {
        current = gPlayerSlots[0]->currentStatB + p->bonus;
        maximum = gPlayerSlots[0]->maximumStatB;
    } else {
        current = (s16)gGameState->unk2c + p->bonus;
        maximum = *(s16 *)gGameState->unk2E;
    }
    sub_08243B54(0, 2, 8, current, maximum, 0x35f);
}
