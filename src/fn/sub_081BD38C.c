#include "global.h"
// MATCHING_CONCESSION: register-pinning

struct Resource {
    u32 words[8];
};
struct Widget {
    u8 unk_00[0x48];
    void *text;
    u8 unk_4C[0x14];
};
struct DisplaySprite {
    u32 flags;
    u8 unk_04[3];
    u8 mode;
    u8 unk_08[0x14];
    s16 x, y, z;
    u8 unk_22[0xA];
};
struct AnimationData {
    u8 unk_00[0x1C];
};
struct ScriptPanel {
    u8 unk_00[0x18];
    struct Resource resource;
    struct Resource secondaryResource;
    struct Widget background;
    struct Widget text;
    struct Widget rateDigit;
    struct Widget decimalDigits[2];
    struct Widget percent;
    struct Widget upperDigits[4];
    struct Widget middleDigits[4];
    struct Widget lowerRateDigit;
    struct Widget lowerDigits[4];
    struct Widget label;
    struct Widget decorations[2];
    struct DisplaySprite sprite;
    struct AnimationData animation;
    u8 unk_8E0[0xC];
    u16 rate;
    u8 unk_8EE[0x22];
    u32 snapshot[8];
};
struct Position {
    s16 x, y, z, w;
};
void *ScriptProc_Start(u16, u16);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, s32, s32, s32, s32, s32, void *);
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, s32);
void AnimData_SelectPaletteBank(void *, u32);
static inline void Position_Set(struct Position *p, s32 x, s32 y, s32 z)
{
    p->x = x;
    p->y = y;
    p->z = z;
}

void DarkLoan_BuildDisplay(struct ScriptPanel *p, s32 alternate)
{
    struct Resource *res;
    struct Resource *savedSecondary;
    struct Resource *primary;
    struct Resource *secondary;
    struct Position pos;
    s32 flags;
    // Keep the widget coordinate zero separate from the packed position reset.
    register s32 zero asm("r1");
    s32 i;

    res = ScriptProc_Start(0xCB05, 0x32BA);
    p->resource = *res;
    ResourceHeader_Relocate(&p->resource, res);
    res = ScriptProc_Start(0xCB05, 0x530D);
    p->secondaryResource = *res;
    savedSecondary = &p->secondaryResource;
    ResourceHeader_Relocate(savedSecondary, res);
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    zero = 0;
    flags = 0x10;
    if (alternate)
        flags = 0x90;
    Widget_InitLinked(&p->background, &p->resource, 4, flags, zero, zero, 0x3C, &pos);
    Widget_InitLinked(&p->rateDigit, &p->resource, (u16)(p->rate + 7), flags, 0, 0, 0x3C, &pos);
    pos.x += 8;
    Widget_InitLinked(&p->decimalDigits[0], &p->resource, 8, flags, 0, 0, 0x3C, &pos);
    pos.x += 8;
    Widget_InitLinked(&p->decimalDigits[1], &p->resource, 8, flags, 0, 0, 0x3C, &pos);
    pos.x = -8;
    pos.y = -8;
    Widget_InitLinked(&p->text, savedSecondary, 0x4B, flags, 0, 0, 0x3C, &pos);
    p->text.text = p->snapshot;
    pos.x = 0x10;
    pos.y = -4;
    Widget_InitLinked(&p->percent, &p->resource, 0x12, flags, 0, 0, 0x3C, &pos);
    pos.x = 0x90;
    pos.y = 0x28;
    primary = &p->resource;
    secondary = savedSecondary;
    for (i = 0; i < 4; i++) {
        struct Position *position = &pos;
        struct Widget *widget = &p->upperDigits[i];
        Widget_InitLinked(widget, secondary, 0x14, flags, 0, 0, 0x3C, position);
        pos.x += 8;
    }
    pos.x = -0x38;
    pos.y = 0x38;
    for (i = 0; i < 4; i++) {
        struct Position *position = &pos;
        struct Widget *widget = &p->middleDigits[i];
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
        struct Position *position = &pos;
        struct Widget *widget = &p->lowerDigits[i];
        Widget_InitLinked(widget, primary, 8, flags, 0, 0, 0x3C, position);
        pos.x += 8;
    }
    AnimData_Load(&p->animation, 0xA47C);
    Sprite_Init(&p->sprite, &p->animation, flags);
    p->sprite.mode = 0;
    AnimData_SelectPaletteBank(&p->animation, 0x1B4);
    p->sprite.x = 0xA0;
    p->sprite.y = 0x58;
    p->sprite.z = 0;
}
