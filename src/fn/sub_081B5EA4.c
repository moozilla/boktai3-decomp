// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_vec.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// Remaining differences: second-loop field cursors, action-state addressing and spills.
void *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, u32, u32, u32, u32, u32, u32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void CpuSet(const void *, void *, u32);
void PairedSceneActor_InitializeHalf(void *, s32, s32);
extern u8 *gUnk_030042E4;
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
static inline void ChangeAction(u8 *p, u32 action)
{
    u32 zero = 0;
    if (*(u32 *)(p + 0x160) & 0x100)
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
inline static void ChangeAction_1(u8 *p)
{
    u32 zero = 0;
    if ((*((u32 *)(p + 0x160))) & 0x100)
        return;
    if ((((u8)(p[0x16d] - 25)) <= 1) && (!p[0x170]))
        return;
    p[0x16d] = 2;
    {
        typeof(p[0x16c]) *q1 = &p[0x16c];
        u8 z1 = 1;
        *q1 = 2;
        p[0x16e] = z1;
        p[0x170] = zero;
        *((u32 *)(p + 0x178)) = zero;
        *((u32 *)(p + 0x164)) = zero;
        *((u32 *)(p + 0x168)) = zero;
    }
}

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
    typeof(half) c1;
    resource = boss + 0x78;
    ResourceHeader_Relocate(resource, data);
    animation = boss + 0x18;
    Widget_InitLinked(animation, resource, 0, 16, 3, 1, 60, 0);
    Anim_SetKeyframes(animation, resource, 0, 1, 0);
    if (!palettes)
        source = gUnk_030042E4 + 0xe00;
    else
        source = palettes + 0x20;
    CpuSet(source, boss + 0x514, 0x04000010);
    CpuSet(source, boss + 0x4d4, 0x04000010);
    source = gUnk_030042E4 + 0x3240;
    CpuSet(source, boss + 0x554, 0x04000008);
    source = gUnk_030042E4 + 0x1060;
    CpuSet(source, boss + 0x574, 0x04000010);
    *((u32 *)(boss + 0x20)) |= 1;
    *((void **)(boss + 0x4d0)) = ScriptProc_Start(0x922e, 0x572b);
    i = 0;
    links = (void **)(boss + 0x228);
    half = boss + 0x98;
    key = (u16 *)(boss + 0xca);
    c1 = half;
    do {
        PairedSceneActor_InitializeHalf(c1, arg, i);
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
            register u32 stride asm("r0") = 0x21c;
            asm("" : "+r"(stride));
            links = (void **)(((u8 *)links) + stride);
            palette = (void **)(((u8 *)palette) + stride);
            key = (u16 *)(((u8 *)key) + stride);
            half += stride;
        }
        palette = (void **)(boss + 0xd0);
        i++;
    } while (i <= 1);
    *((u32 *)(boss + 0x5b4)) = 0;
    switch (mode) {
        case 0:
            *((u16 *)(boss + 0x20c)) = 0;
            *((u16 *)(boss + 0x428)) = 0;
            break;

        case 1:
            half = boss + 0x98;
            do {
                *((u16 *)(half + 0x174)) = *((u16 *)(half + 0x176));
                *((u32 *)half) &= ~1;
                CpuSet(*((void **)(half + 0x190)), *((void **)(half + 0x38)), 0x04000008);
                Vector((struct SceneVec *)(half + 0x9c), *((s16 *)(half + 0xac)),
                       *((s16 *)(half + 0xae)));
                ChangeAction_1(half);
                half += 0x21c;
            } while (half <= (boss + 0x2b4));
            break;
    }

    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081B5EA4);
#endif
