#include "global.h"
#include "engine/video/render.h"
#include "engine/video/anim_sprite.h"
#include "engine/video/display_sequence_step.h"
#include "engine/video/sprite.h"
#include "scene/collision/contact_position.h"

void CpuSet(const void *src, void *dst, u32 control);
s32 AnimSprite_InitLinked(void *, void *, u16, u32, u32, u32, u32, const void *);
s32 Anim_LoadKeyframe(void *, void *, u16, u8);
void Anim_SetKeyframes(void *, void *, u16, u16, u16);
u32 Anim_SetSequence(void *, void *, u16, u8, u8);
void *ScriptProc_Start(u16, u16);
s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void AnimData_SelectPaletteBank(void *, s32);

#define WI(w, par, id, type, a, b, pos)                                                            \
    AnimSprite_InitLinked((w), (par), (id), (type), (a), (b), 60, (pos))

struct HudLayout {
    u8 unknown0[0x64];
    struct AnimSpriteResource resources[4];
    u8 unknownE4[0x104 - 0xE4];
    struct AnimSpriteResource actionResource;
    u8 unknown124[0x40];
    struct AnimSpriteResource comparisonResource;
    struct DisplaySequenceSprite sprite;
    u8 spriteResource[0x1c];
    struct DisplaySequenceState animation;
    void *script;
    u8 unknown1E0[0xC28 - 0x1E0];
    struct DisplaySequenceSprite comparisonSprite;
    u8 comparisonSpriteResource[0x1c];
    u8 unknownC70[0x1420 - 0xC70];
    struct AnimSprite widgets[124];
    u8 unknown42A0[4];
    struct AnimSprite compareAction, compareUnlock;
    u8 unknown4364[0x50];
    u16 palette[16];
};
static inline s32 HudLayout_InitWidget(struct HudLayout *p, s32 index, void *par, u32 id, u32 type,
                                       u32 a, u32 b, const void *pos)
{
    return AnimSprite_InitLinked(&p->widgets[index], par, id, type, a, b, 60, pos);
}

static inline void ClearPosition(u32 value, struct ContactPosition *position)
{
    CpuFill32(value, position, sizeof(*position));
}

void sub_08176F70(struct HudLayout *p)
{
    struct ContactPosition v;
    u8 *w;
    s32 i;
    struct DisplaySequenceState *anim;

    {
        u8 *w;
        u32 initial = 0;
        struct ContactPosition *position = &v;
        u32 defaults = 0;
        ClearPosition(initial, position);
        WI(&p->widgets[0], &p->resources[0], 0x51, 0xB0, initial, initial, position);
        // The original writes `initial` again here without reading it; the
        // write ends the value's use, so the later zeros come from `defaults`.
        initial = 1;
        WI(&p->widgets[1], &p->resources[0], 0x43, 0xB1, defaults, defaults, position);
        WI(&p->widgets[2], &p->resources[0], 0x44, 0xB1, defaults, defaults, position);
        WI(&p->widgets[8], &p->resources[0], 0x63, 0xB1, defaults, defaults, position);
        WI(&p->widgets[9], &p->resources[0], 0x70, 0xB1, defaults, defaults, position);
        WI(&p->widgets[3], &p->resources[0], 0x40, 0xB1, defaults, defaults, position);
        WI(&p->widgets[4], &p->resources[0], 0x4D, 0xB1, defaults, defaults, position);
        WI(&p->widgets[6], &p->resources[0], 0x4E, 0xB1, defaults, defaults, position);
        w = (u8 *)&p->widgets[54];
        WI(w, &p->resources[0], 0x4D, 0xB1, defaults, defaults, position);
        Anim_LoadKeyframe(w, &p->resources[0], 0x4D, 1);
        WI(&p->widgets[5], &p->resources[0], 0x4D, 0xB1, defaults, defaults, position);
        WI(&p->widgets[75], &p->resources[0], 0x7F, 0xB1, defaults, defaults, position);
        WI(&p->widgets[76], &p->resources[0], 0x7F, 0xB1, defaults, defaults, position);
    }
    for (i = 0; i <= 0x13; i++) {
        WI((&p->widgets[10 + i]), &p->resources[2], 0x68, 0xB1, 0, 0, &v);
    }
    for (i = 0; i <= 0xF; i++) {
        WI((&p->widgets[30 + i]), &p->resources[0], 0x50, 0xB1, 0, 0, &v);
    }
    for (i = 0; i <= 3; i++) {
        WI((&p->widgets[50 + i]), &p->resources[2], 0x68, 0xB1, 0, 0, &v);
        WI((&p->widgets[55 + i]), &p->resources[0], 0x50, 0xB1, 0, 0, &v);
    }
    v.x = 0xD8;
    v.y = 0x20;
    WI(&p->widgets[46], &p->resources[2], 0x193, 0xB1, 0, 0, &v);
    v.x = 0xD8;
    v.y = 0x38;
    WI(&p->widgets[47], &p->resources[2], 0x192, 0xB1, 0, 0, &v);
    v.x = 0xD8;
    v.y = 0x50;
    WI(&p->widgets[48], &p->resources[2], 0x191, 0xB1, 0, 0, &v);
    v.x = 0xD8;
    v.y = 0x68;
    WI(&p->widgets[49], &p->resources[2], 0x190, 0xB1, 0, 0, &v);
    v.x = 0;
    v.y = 0x80;
    WI(&p->widgets[59], &p->resources[2], 0xCF, 0xB1, 0, 0, &v);
    for (i = 0; i <= 1; i++) {
        WI((&p->widgets[60 + i]), &p->resources[2], 0x68, 0xB1, 0, 0, &v);
        WI((&p->widgets[122 + i]), &p->resources[0], 0x50, 0xB1, 0, 0, &v);
    }
    v.x = 0x34;
    v.y = 0x64;
    WI(&p->widgets[74], &p->actionResource, 0, 0x94, 0, 0, &v);
    Anim_SetKeyframes(&p->widgets[74], &p->actionResource, 0xD, 1, 0);
    v.x = 0;
    v.y = 0;
    for (i = 0; i <= 0xB; i++) {
        WI((&p->widgets[62 + i]), &p->resources[0], 0xA, 0xB1, 0, 0, &v);
    }
    v.x = 0;
    v.y = 0;
    WI(&p->widgets[7], &p->resources[0], 0x4B, 0x91, 0, 0, &v);
    p->script = ScriptProc_Start(0x922E, 0x4BBF);
    AnimData_Load(p->spriteResource, 0x593A);
    Sprite_Init(&p->sprite, p->spriteResource, 0xD1);
    p->sprite.priority = 0;
    anim = &p->animation;
    Anim_SetSequence(anim, p->script, 0, 0, 0);
    DisplaySequence_StepSprite(&p->sprite, anim);

    p->sprite.position.x = 0;
    p->sprite.position.y = 0;
    p->sprite.position.z = 0;
    v.x = 0;
    v.y = 0;
    WI(&p->widgets[77], &p->resources[0], 0x62, 0x91, 0, 0, &v);
    v.x = 0;
    v.y = 0;
    WI(&p->widgets[78], &p->resources[0], 0x77, 0x91, 0, 0, &v);
    v.x = 0;
    v.y = 0;
    for (i = 0; i <= 3; i++) {
        struct ContactPosition *position = &v;
        w = (u8 *)&p->widgets[79 + i];
        WI(w, &p->resources[0], 0x5B, 0x91, 0, 0, position);
        v.y += 0xD;
    }
    v.x = 0;
    v.y = 0;
    WI(&p->widgets[83], &p->resources[0], 0x7D, 0x91, 0, 0, &v);
    WI(&p->widgets[84], &p->resources[0], 0x7E, 0x91, 0, 0, &v);
    v.x = 0;
    v.y = 0;
    for (i = 0; i <= 2; i++) {
        v.z = 0;
        HudLayout_InitWidget(p, 85 + i, &p->resources[0], 0x79, 0x91, 1, 0, &v);
        p->widgets[85 + i].attribute1A = 3;
        v.z -= 8;
        WI((&p->widgets[88 + i]), &p->resources[0], 0x7A, 0x91, 1, 0, &v);
        p->widgets[88 + i].attribute1A = 3;
        v.z -= 8;
        WI((&p->widgets[91 + i]), &p->resources[0], 0x7B, 0x91, 1, 0, &v);
        p->widgets[91 + i].attribute1A = 3;
        v.y += 8;
    }
    v.x = 0;
    v.y = 0;
    for (i = 0; i <= 3; i++) {
        WI((&p->widgets[98 + i]), &p->resources[1], 4 + i * 2, 0x91, 1, 0, &v);
        Anim_SetKeyframes((&p->widgets[98 + i]), &p->resources[1], i, 1, 0);
    }
    p->widgets[100].position.x += 0x10;
    WI(&p->widgets[96], &p->resources[1], 0, 0x91, 1, 0, &v);
    WI(&p->widgets[97], &p->resources[1], 1, 0x91, 1, 0, &v);

    v.x = 0;
    v.y = 0;
    WI(&p->widgets[102], &p->resources[0], 0x75, 0xB0, 0, 0, &v);
    WI(&p->widgets[103], &p->resources[0], 0x76, 0xB0, 0, 0, &v);
    WI(&p->widgets[104], &p->resources[0], 0x73, 0xB0, 0, 0, &v);
    WI(&p->widgets[105], &p->resources[0], 0x74, 0xB0, 0, 0, &v);
    v.x = 0;
    v.y = 0;
    WI(&p->widgets[94], &p->resources[0], 0x45, 0xB1, 0, 0, &v);
    WI(&p->widgets[95], &p->resources[0], 0x46, 0xB1, 0, 0, &v);
    v.x = 0;
    v.y = 0;
    WI(&p->widgets[106], &p->resources[3], 4, 0xB0, 0, 0, &v);
    WI(&p->widgets[107], &p->resources[3], 0x19, 0xB0, 0, 0, &v);
    WI(&p->widgets[108], &p->resources[3], 0x1B, 0xB0, 0, 0, &v);
    v.x = 0;
    v.y = 0;
    for (i = 0; i <= 2; i++) {
        WI((&p->widgets[110 + i]), &p->resources[3], 0, 0xB0, 0, 0, &v);
        v.x += 8;
    }
    v.x = 0x10;
    v.y = -4;
    WI(&p->widgets[109], &p->resources[3], 0x12, 0xB0, 0, 0, &v);
    v.x = -0x38;
    v.y = 0x38;
    for (i = 0; i <= 3; i++) {
        WI((&p->widgets[113 + i]), &p->resources[3], 0, 0xB0, 0, 0, &v);
        v.x += 8;
    }
    v.x = 0x58;
    v.y = 0x38;
    WI(&p->widgets[117], &p->resources[3], 0, 0xB0, 0, 0, &v);
    v.x = -0x30;
    v.y = 0x48;
    for (i = 0; i <= 3; i++) {
        WI((&p->widgets[118 + i]), &p->resources[3], 0, 0xB0, 0, 0, &v);
        v.x += 8;
    }
    w = p->comparisonSpriteResource;
    AnimData_Load(w, 0xA47C);
    Sprite_Init(&p->comparisonSprite, w, 0x91);
    AnimData_SelectPaletteBank(w, 0x1B4);
    p->comparisonSprite.priority = 0;
    p->comparisonSprite.position.x = 0xA0;
    p->comparisonSprite.position.y = 0x58;
    p->comparisonSprite.position.z = 0;
    {
        struct AnimSprite *unlock = &p->compareUnlock;
        struct AnimSpriteResource *resource = &p->comparisonResource;
        AnimSprite_InitLinked(unlock, resource, 2, 0x91, 0, 0, 0, 0);
        AnimSprite_InitLinked(&p->compareAction, resource, 0, 0x91, 0, 0, 0, 0);
    }
    CpuSet(gPaletteBankData + 0x7540, p->palette, 0x04000008);
}
