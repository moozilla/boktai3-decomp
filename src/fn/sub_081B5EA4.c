// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_vec.h"
#include "render.h"
void *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, u32, u32, u32, u32, u32, u32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void CpuSet(const void *, void *, u32);
void PairedSceneActor_InitializeHalf(void *, s32, s32);
extern const s16 gUnk_086149C4[];
static inline s32 Scale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void Vector(struct SceneVec *v, s32 angle, s32 scale)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * scale);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle & 255] * scale);
}
static inline u32 MaskedFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline void ChangeAction(u8 *p, u32 action)
{
    u32 zero = 0;
    if (MaskedFlags((u32 *)(p + 0x160), 0x100))
        return;
    if ((u8)(p[0x16d] - 25) <= 1 && !p[0x170])
        return;
    p[0x16d] = action;
    p[0x16c] = action;
    p[0x16e] = 1;
    p[0x170] = zero;
    *(u32 *)(p + 0x178) = zero;
    *(u32 *)(p + 0x164) = zero;
    *(u32 *)(p + 0x168) = zero;
}
// confidence=low: initializes the controller resources and its two embedded halves.
#define Vanargand_InitializePartController sub_081B5EA4
s32 Vanargand_InitializePartController(u8 *boss, s32 arg, u8 *palettes, s32 mode)
{
    u8 *resource;
    u8 *animation;
    u8 *source;
    u8 *half;
    void **links;
    void **palette;
    u16 *key;
    s32 i;
    void *data = ScriptProc_Start(0xcb05, 0xb3de);
    resource = boss + 0x78;
    ResourceHeader_Relocate(resource, data);
    animation = boss + 0x18;
    Widget_InitLinked(animation, resource, 0, 16, 3, 1, 60, 0);
    Anim_SetKeyframes(animation, resource, 0, 1, 0);
    if (!palettes)
        source = gPaletteBankData + 0xe00;
    else
        source = palettes + 0x20;
    CpuSet(source, boss + 0x514, 0x04000010);
    CpuSet(source, boss + 0x4d4, 0x04000010);
    source = gPaletteBankData + 0x3240;
    CpuSet(source, boss + 0x554, 0x04000008);
    source = gPaletteBankData + 0x1060;
    CpuSet(source, boss + 0x574, 0x04000010);
    *((u32 *)(boss + 0x20)) |= 1;
    *((void **)(boss + 0x4d0)) = ScriptProc_Start(0x922e, 0x572b);
    i = 0;
    links = (void **)(boss + 0x228);
    palette = (void **)(boss + 0xd0);
    key = (u16 *)(boss + 0xca);
    half = boss + 0x98;
    do {
        PairedSceneActor_InitializeHalf(half, arg, i);
        if (i == 1) {
            *key = 0x70;
            links[0] = boss + 0x514;
            links[2] = boss + 0x574;
            if (!palettes)
                *palette = boss + 0x4f4;
            else
                *palette = palettes + 0x20;
        } else {
            *key = 0x71;
            links[0] = boss + 0x534;
            links[2] = boss + 0x594;
            if (!palettes)
                *palette = boss + 0x4d4;
            else
                *palette = palettes + 0x40;
        }
        links[1] = boss + 0x554;
        CpuSet(links[0], *palette, 0x04000008);
        {
            union {
                u32 v;
            } stride = {0x21c};
            links = (void **)(((u8 *)links) + stride.v);
            palette = (void **)(((u8 *)palette) + stride.v);
            key = (u16 *)(((u8 *)key) + stride.v);
            half += stride.v;
        }
        i++;
    } while (i <= 1);
    {
        register u32 offset asm("r2") = 0x5b4;
        u32 *state;
        state = (u32 *)(boss + offset);
        asm("" : : "r"(offset));
        *state = 0;
    }
    switch (mode) {
        case 0:
            *((u16 *)(boss + 0x20c)) = 0;
            *((u16 *)(boss + 0x428)) = 0;
            break;

        case 1:
            i = 0;
            do {
                *((u16 *)(boss + i * 0x21c + 0x20c)) = *((u16 *)(boss + i * 0x21c + 0x20e));
                *((u32 *)(boss + i * 0x21c + 0x98)) &= ~1;
                CpuSet(*((void **)(boss + i * 0x21c + 0x228)),
                       *((void **)(boss + i * 0x21c + 0xd0)), 0x04000008);
                Vector((struct SceneVec *)(boss + i * 0x21c + 0x134),
                       *((s16 *)(boss + i * 0x21c + 0x144)), *((s16 *)(boss + i * 0x21c + 0x146)));
                ChangeAction(boss + i * 0x21c + 0x98, 2);
                i++;
            } while (i <= 1);
            break;
    }

    return 0;
}
