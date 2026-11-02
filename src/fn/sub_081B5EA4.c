#include "global.h"
#include "paired_scene_actor.h"
#include "render.h"
void *ScriptProc_Start(u16, u16);
s32 ResourceHeader_Relocate(void *, void *);
s32 Widget_InitLinked(void *, void *, u16, u32, u8, u8, u8, const void *);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void CpuSet(const void *, void *, u32);
extern const s16 gUnk_086149C4[];
static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void Vector(struct ContactPosition *v, s32 angle, s32 scale)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * scale);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * scale);
}
static inline u32 MaskedFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline void ChangeAction(struct PairedSceneHalf *p, u32 action)
{
    u32 zero = 0;
    if (MaskedFlags(&p->actionFlags, 0x100))
        return;
    if ((u8)(p->requestedState - 25) <= 1 && !p->finished)
        return;
    p->requestedState = action;
    p->state = action;
    p->entered = 1;
    p->finished = zero;
    p->stateTimer = zero;
    p->actionCounters[0] = zero;
    p->actionCounters[1] = zero;
}
// confidence=low: initializes the controller resources and its two embedded halves.
#define Vanargand_InitializePartController sub_081B5EA4
s32 Vanargand_InitializePartController(struct VanargandAttackPair *boss, void *origin,
                                       u16 (*palettes)[16], s32 mode)
{
    struct SceneInputResource *resource;
    struct SceneInputWidget *animation;
    const void *source;
    s32 i;
    void *data = ScriptProc_Start(0xcb05, 0xb3de);
    resource = &boss->resource;
    ResourceHeader_Relocate(resource, data);
    animation = &boss->widget;
    Widget_InitLinked(animation, resource, 0, 16, 3, 1, 60, 0);
    Anim_SetKeyframes(animation, resource, 0, 1, 0);
    if (!palettes)
        source = gPaletteBankData + 0xe00;
    else
        source = palettes[1];
    CpuSet(source, boss->palettes[2], 0x04000010);
    CpuSet(source, boss->palettes[0], 0x04000010);
    source = gPaletteBankData + 0x3240;
    CpuSet(source, boss->palettes[4], 0x04000008);
    source = gPaletteBankData + 0x1060;
    CpuSet(source, boss->palettes[5], 0x04000010);
    boss->widget.flags |= 1;
    boss->effectResource = ScriptProc_Start(0x922e, 0x572b);
    for (i = 0; i < 2; i++) {
        struct PairedSceneHalf *part = &boss->halves[i];

        PairedSceneActor_InitializeHalf(part, origin, i);
        if (i == 1) {
            part->graphics.paletteIndex = 0x70;
            part->palettes[0] = boss->palettes[2];
            part->palettes[2] = boss->palettes[5];
            if (!palettes)
                part->graphics.palette = boss->palettes[1];
            else
                part->graphics.palette = palettes[1];
        } else {
            part->graphics.paletteIndex = 0x71;
            part->palettes[0] = boss->palettes[3];
            part->palettes[2] = boss->palettes[6];
            if (!palettes)
                part->graphics.palette = boss->palettes[0];
            else
                part->graphics.palette = palettes[2];
        }
        part->palettes[1] = boss->palettes[4];
        CpuSet(part->palettes[0], part->graphics.palette, 0x04000008);
    }
    boss->selectedMask = 0;
    switch (mode) {
        case 0:
            boss->halves[0].health = 0;
            boss->halves[1].health = 0;
            break;

        case 1:
            i = 0;
            do {
                struct PairedSceneHalf *part = &boss->halves[i];
                boss->halves[i].health = boss->halves[i].maxHealth;
                part->sprite.flags &= ~1;
                CpuSet(part->palettes[0], part->graphics.palette, 0x04000008);
                Vector(&boss->halves[i].offset, (s16)boss->halves[i].heading,
                       (s16)boss->halves[i].scale);
                ChangeAction(&boss->halves[i], 2);
                i++;
            } while (i <= 1);
            break;
    }

    return 0;
}
