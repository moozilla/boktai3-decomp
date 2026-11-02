#include "dark_loan_screen.h"

void *ScriptProc_Start(u16, u16);
s32 Widget_InitLinked(void *, void *, u16, u32, u8, u8, u8, const struct ContactPosition *);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, s32);
void AnimData_SelectPaletteBank(void *, u32);
static inline void Position_Set(struct ContactPosition *p, s32 x, s32 y, s32 z)
{
    p->x = x;
    p->y = y;
    p->z = z;
}

void DarkLoan_BuildDisplay(struct DarkLoanScreen *p, s32 alternate)
{
    struct DisplayResourceHeader *res;
    struct DisplayResourceHeader *savedSecondary;
    struct DisplayResourceHeader *primary;
    struct DisplayResourceHeader *secondary;
    struct ContactPosition pos;
    s32 flags;
    struct SceneWidgetPlayback backgroundPlayback;
    u8 backgroundDefault;
    s32 i;

    res = ScriptProc_Start(0xCB05, 0x32BA);
    p->resource = *res;
    ResourceHeader_Relocate(&p->resource, res);
    res = ScriptProc_Start(0xCB05, 0x530D);
    p->secondaryResource = *res;
    savedSecondary = &p->secondaryResource;
    ResourceHeader_Relocate(savedSecondary, res);
    backgroundPlayback.priority = 0;
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    backgroundDefault = backgroundPlayback.priority;
    flags = 0x10;
    if (alternate)
        flags = 0x90;
    Widget_InitLinked(&p->background, &p->resource, 4, flags, backgroundDefault, backgroundDefault,
                      0x3C, &pos);
    Widget_InitLinked(&p->rateDigit, &p->resource, (u16)(p->rate + 7), flags, 0, 0, 0x3C, &pos);
    pos.x += 8;
    Widget_InitLinked(&p->decimalDigits[0], &p->resource, 8, flags, 0, 0, 0x3C, &pos);
    pos.x += 8;
    Widget_InitLinked(&p->decimalDigits[1], &p->resource, 8, flags, 0, 0, 0x3C, &pos);
    pos.x = -8;
    pos.y = -8;
    Widget_InitLinked(&p->text, savedSecondary, 0x4B, flags, 0, 0, 0x3C, &pos);
    p->text.palette = (u16 *)p->snapshot;
    pos.x = 0x10;
    pos.y = -4;
    Widget_InitLinked(&p->percent, &p->resource, 0x12, flags, 0, 0, 0x3C, &pos);
    pos.x = 0x90;
    pos.y = 0x28;
    primary = &p->resource;
    secondary = savedSecondary;
    for (i = 0; i < 4; i++) {
        struct ContactPosition *position = &pos;
        struct SceneInputWidget *widget = &p->upperDigits[i];
        Widget_InitLinked(widget, secondary, 0x14, flags, 0, 0, 0x3C, position);
        pos.x += 8;
    }
    pos.x = -0x38;
    pos.y = 0x38;
    for (i = 0; i < 4; i++) {
        struct ContactPosition *position = &pos;
        struct SceneInputWidget *widget = &p->middleDigits[i];
        Widget_InitLinked(widget, primary, 8, flags, 0, 0, 0x3C, position);
        pos.x += 8;
    }
    Widget_InitLinked(&p->label, primary, 0x22, flags, 0, 0, 0x3C, 0);
    Position_Set(&pos, -0x85, -0x18, 0);
    Widget_InitLinked(&p->decorations[0], secondary, 0x7E, flags, 0, 0, 0x3C, &pos);
    Widget_InitLinked(&p->decorations[1], secondary, 0x7F, flags, 0, 0, 0x3C, &pos);
    pos.x = 0x58;
    pos.y = 0x38;
    Widget_InitLinked(&p->lowerRateDigit, primary, (u16)(p->rate + 7), flags, 0, 0, 0x3C, &pos);
    pos.x = -0x30;
    pos.y = 0x48;
    for (i = 0; i < 4; i++) {
        struct ContactPosition *position = &pos;
        struct SceneInputWidget *widget = &p->lowerDigits[i];
        Widget_InitLinked(widget, primary, 8, flags, 0, 0, 0x3C, position);
        pos.x += 8;
    }
    AnimData_Load(p->animation, 0xA47C);
    Sprite_Init(&p->sprite, p->animation, flags);
    p->sprite.priority = 0;
    AnimData_SelectPaletteBank(p->animation, 0x1B4);
    p->sprite.position.x = 0xA0;
    p->sprite.position.y = 0x58;
    p->sprite.position.z = 0;
}
