#include "scene_input.h"
#include "sun_clock.h"

struct BankAnim {
    u8 b[0x1C];
};
struct SolarBankDisplay {
    u8 pad0[0x18];
    struct SceneInputResource bankResource;
    struct SceneInputResource commonResource;
    struct SceneInputWidget label;
    struct SceneInputWidget prompt;
    struct SceneInputWidget rateDigits[2];
    struct SceneInputWidget percentSign;
    struct SceneInputWidget balanceDigits[4];
    struct SceneInputWidget transferDigits[4];
    struct SceneInputWidget transferLabel;
    struct SceneInputWidget firstMarker;
    struct SceneInputWidget secondMarker;
    struct DisplaySequenceSprite sprites[2];
    struct BankAnim anims[2];
    u8 unknown6E8[0x28];
    u16 promptPalette[16];
};

static inline void Pos_Set(struct ContactPosition *p, s32 x, s32 y, s32 z)
{
    p->x = x;
    p->y = y;
    p->z = z;
}

s32 Mod(s32, s32);
s32 Div(s32, s32);

static inline void SolarBank_SetRate(s16 *rate, s32 level)
{
    *rate = 0x41 + level;
}

#define SolarBank_InitWidgets sub_081BBE88
void SolarBank_InitWidgets(struct SolarBankDisplay *p)
{
    struct SceneWidgetPlayback labelPlayback;
    u8 labelPriority;
    struct SceneInputResource *source;
    struct SceneInputResource *commonResource;
    struct SceneInputResource *bankResource;
    struct ContactPosition pos;
    s32 percent;
    s32 tens;
    s32 i;
    source = ScriptProc_Start(0xCB05, 0x32BA);
    p->bankResource = *source;
    ResourceHeader_Relocate(&p->bankResource, source);
    source = ScriptProc_Start(0xCB05, 0x530D);
    p->commonResource = *source;
    ResourceHeader_Relocate(&p->commonResource, source);
    labelPlayback.priority = 0;
    labelPlayback.mode = 0;
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    labelPriority = labelPlayback.priority;
    Widget_InitLinked(&p->label, &p->bankResource, 0, 0x30, labelPriority, labelPlayback.mode, 60,
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
    Widget_InitLinked(&p->rateDigits[0], &p->bankResource, (u16)(tens + 8), 0x30, 0, 0, 0x3C, &pos);
    pos.x += 8;
    Widget_InitLinked(&p->rateDigits[1], &p->bankResource, (u16)(percent + 8), 0x30, 0, 0, 0x3C,
                      &pos);
    pos.x = 0;
    Widget_InitLinked(&p->percentSign, &p->bankResource, 0x12, 0x30, 0, 0, 0x3C, &pos);
    pos.x = -8;
    pos.y = -8;
    Widget_InitLinked(&p->prompt, &p->commonResource, 0x4B, 0x10, 0, 0, 0x3C, &pos);
    p->prompt.palette = p->promptPalette;
    pos.x = 0x28;
    pos.y = 0x28;
    bankResource = &p->bankResource;
    commonResource = &p->commonResource;
    for (i = 0; i < 4; i++) {
        struct ContactPosition *position = &pos;
        struct SceneInputWidget *widget = &p->balanceDigits[i];
        Widget_InitLinked(widget, commonResource, 0, 0x30, 0, 0, 0x3C, position);
        pos.x += 8;
    }

    pos.x = 0x88;
    pos.y = 0x28;
    for (i = 0; i < 4; i++) {
        struct ContactPosition *position = &pos;
        struct SceneInputWidget *widget = &p->transferDigits[i];
        Widget_InitLinked(widget, commonResource, 0, 0x30, 0, 0, 0x3C, position);
        pos.x += 8;
    }

    for (i = 0; i < 2; i++) {
        AnimData_Load(&p->anims[i], 0xA47C);
        Sprite_Init(&p->sprites[i], &p->anims[i], 0);
        p->sprites[i].priority = 0;
        p->sprites[i].flags = 0x30;
        if (i == 0)
            p->sprites[0].flags |= 4;
        if (i == 1)
            AnimData_SelectPaletteBank(&p->anims[1], 0x1B1);
        p->sprites[i].position.x = 0x38 + (i * 0x60);
        p->sprites[i].position.y = 0x58;
        p->sprites[i].position.z = 0;
    }

    Widget_InitLinked(&p->transferLabel, bankResource, 0x21, 0x30, 0, 0, 0x3C, (void *)0);
    Pos_Set(&pos, -0x85, -0x18, 0);
    Widget_InitLinked(&p->firstMarker, commonResource, 0x7E, 0x30, 0, 0, 0x3C, &pos);
    Widget_InitLinked(&p->secondMarker, commonResource, 0x7F, 0x30, 0, 0, 0x3C, &pos);
}
