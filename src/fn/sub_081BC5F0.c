#define gPlayerSlots gUnk_02000580
#include "global.h"
#include "player.h"
#include "game_state.h"
void StatGauge_Draw(s32, s32, s32, s32, s32, s32);
struct StatGaugePreview {
    u8 unknown0[0x8ee];
    u16 bonus;
    u8 unknown8f0[4];
    u32 exitScript;
};
void StatGauge_DrawPreview(struct StatGaugePreview *p)
{
    s32 current, maximum;
    if (p->exitScript == 0) {
        current = gPlayerSlots[0]->currentStatB + p->bonus;
        maximum = gPlayerSlots[0]->maximumStatB;
    } else {
        current = (s16)gGameState->unk2c + p->bonus;
        maximum = *(s16 *)gGameState->unk2E;
    }
    StatGauge_Draw(0, 2, 8, current, maximum, 0x35f);
}
