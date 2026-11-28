#include "ui/town/solar_bank_panel.h"
#include "ui/widgets/scene_input.h"
#include "system/clock/sun_clock.h"
#include "bosses/vanargand/vanargand_parts.h"

s32 Mod(s32, s32);
s32 Div(s32, s32);

static inline void SolarBank_SetRate(s16 *rate, s32 level)
{
    *rate = 0x41 + level;
}

void SolarBank_InitWidgets(struct SolarBankPanel *p)
{
    struct AnimSpritePlayback labelPlayback;
    u8 labelPriority;
    struct AnimSpriteResource *source;
    struct AnimSpriteResource *commonResource;
    struct AnimSpriteResource *bankResource;
    struct ContactPosition pos;
    s32 percent;
    s32 tens;
    s32 i;
    source = ScriptProc_Start(0xCB05, 0x32BA);
    p->labelData = *source;
    ResourceHeader_Relocate(&p->labelData, source);
    source = ScriptProc_Start(0xCB05, 0x530D);
    p->animData = *source;
    ResourceHeader_Relocate(&p->animData, source);
    labelPlayback.priority = 0;
    labelPlayback.mode = 0;
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    labelPriority = labelPlayback.priority;
    AnimSprite_InitLinked(&p->label, &p->labelData, 0, 0x30, labelPriority, labelPlayback.mode, 60,
                          &pos);
    if (Script_SeekToKeyword('R')) {
        s32 rate = Script_GetValue();
        if (rate < 0)
            rate = 0;
        if (rate > 13)
            rate = 13;
        SolarBank_SetRate(&gGameState->solarBankRate, rate);
    } else if (gGameState->solarBankRate == 0) {
        u32 second = SunClock_GetSecond();
        SolarBank_SetRate(&gGameState->solarBankRate, Mod((u32)gGameState->unk614 ^ second, 13));
    }
    percent = ((100 * gGameState->solarBankRate) >> 6) - 100;
    tens = Div(percent, 10);
    percent -= tens * 10;
    AnimSprite_InitLinked(&p->controls[0], &p->labelData, (u16)(tens + 8), 0x30, 0, 0, 0x3C, &pos);
    pos.x += 8;
    AnimSprite_InitLinked(&p->controls[1], &p->labelData, (u16)(percent + 8), 0x30, 0, 0, 0x3C,
                          &pos);
    pos.x = 0;
    AnimSprite_InitLinked(&p->percentSign, &p->labelData, 0x12, 0x30, 0, 0, 0x3C, &pos);
    pos.x = -8;
    pos.y = -8;
    AnimSprite_InitLinked(&p->prompt, &p->animData, 0x4B, 0x10, 0, 0, 0x3C, &pos);
    p->prompt.palette = p->promptPalette;
    pos.x = 0x28;
    pos.y = 0x28;
    bankResource = &p->labelData;
    commonResource = &p->animData;
    for (i = 0; i < 4; i++) {
        struct ContactPosition *position = &pos;
        AnimSprite_InitLinked(&p->balanceDigits[i], commonResource, 0, 0x30, 0, 0, 0x3C, position);
        pos.x += 8;
    }

    pos.x = 0x88;
    pos.y = 0x28;
    for (i = 0; i < 4; i++) {
        struct ContactPosition *position = &pos;
        AnimSprite_InitLinked(&p->transferDigits[i], commonResource, 0, 0x30, 0, 0, 0x3C, position);
        pos.x += 8;
    }

    for (i = 0; i < 2; i++) {
        AnimData_Load(&p->indicators[i], 0xA47C);
        Sprite_Init(&p->sprites[i], &p->indicators[i], 0);
        p->sprites[i].priority = 0;
        p->sprites[i].flags = 0x30;
        if (i == 0)
            p->sprites[0].flags |= 4;
        if (i == 1)
            AnimData_SelectPaletteBank(&p->indicators[1], 0x1B1);
        p->sprites[i].position.x = 0x38 + (i * 0x60);
        p->sprites[i].position.y = 0x58;
        p->sprites[i].position.z = 0;
    }

    AnimSprite_InitLinked(&p->transferLabel, bankResource, 0x21, 0x30, 0, 0, 0x3C, (void *)0);
    Vanargand_SetVector(&pos, -0x85, -0x18, 0);
    AnimSprite_InitLinked(&p->firstMarker, commonResource, 0x7E, 0x30, 0, 0, 0x3C, &pos);
    AnimSprite_InitLinked(&p->secondMarker, commonResource, 0x7F, 0x30, 0, 0, 0x3C, &pos);
}
