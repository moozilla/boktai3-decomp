// MATCHING_CONCESSION: register-pinning
#include "global.h"

struct Pair {
    u32 a, b;
};
struct SceneEffectInitActor {
    void *update;
    void *destroy;
    u8 resource[0x20];
    u8 anim[8];
    u32 flags;
    u8 pad34[0x88 - 0x34];
    u8 shadow[0xf4 - 0x88];
    u8 collider[0x136 - 0xf4];
    u8 facing;
    u8 pad137[0x148 - 0x137];
    u8 pad148[0x154 - 0x148];
    u8 entered, timer, done, state;
};
struct AtkData {
    u8 pad[0x20];
    u16 a, b;
    u8 pad2[4];
    u16 c, d;
};
extern const struct Pair gUnk_0824F6A0, gUnk_0824F6B0, gUnk_0824F6A8;
void *ScriptProc_Start(u16, u16);
s32 ResourceHeader_Relocate(void *, void *);
void Widget_InitLinked(void *, void *, int, int, int, int, int, const void *);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void ScaledShadow_InitializeDefault(void *, void *);
void ScaledShadow_Disable(void *);
void Collider_Init(void *, u32, u32, u32, u32, const void *, const void *);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
void SceneEffect_UpdateActor(void *);
void SceneEffect_DestroyActor(void *);

static inline u8 Facing(s32 flip)
{
    if (flip)
        return 0x60;
    return 0xe0;
}

void SceneEffect_InitActor(struct SceneEffectInitActor *a, s32 mode, s32 flip)
{
    void *res = ScriptProc_Start(0xCB05, 0x658);
    u8 *anim = (u8 *)a + 8;
    u8 *widget;
    u8 *col;
    u8 *col0;
    u16 k2;
    s16 k3;
    void *update, *destroy;
    ResourceHeader_Relocate(anim, res);
    widget = (u8 *)a + 0x28;
    Widget_InitLinked(widget, anim, 0, 0, 2, 0, 0x3c, &gUnk_0824F6A0);
    Anim_SetKeyframes(widget, anim, 5, 1, 0);
    if (flip)
        a->flags |= 4;
    else
        a->flags &= ~4;
    ScaledShadow_InitializeDefault((u8 *)a + 0x88, (u8 *)a + 0x48);
    ScaledShadow_Disable((u8 *)a + 0x88);
    switch (mode) {
        case 0:
            a->state = mode;
            a->entered = mode;
            a->timer = mode;
            col0 = (u8 *)a + 0xf4;
            Collider_Init(col0, 0, 0x2105, 0, 0x10, &gUnk_0824F6A0, &gUnk_0824F6A0);
            Collider_SetAttackProperties(col0, ((struct AtkData *)*(u8 **)((u8 *)a + 0x150))->a,
                                         ((struct AtkData *)*(u8 **)((u8 *)a + 0x150))->b, 0x10,
                                         mode, mode);
            {
                const struct Pair *src = &gUnk_0824F6A8;
                u32 hi = src->b;
                *(u32 *)(col0 + 0x1c) = src->a;
                *(u32 *)(col0 + 0x20) = hi;
            }
            break;
        case 1:
            k3 = 3;
            k2 = k3;
            a->state = k2;
            a->entered = 0;
            a->timer = 0;
            Collider_Init((u8 *)a + 0xf4, 0, 0x2105, 0, 0x10, &gUnk_0824F6A0, &gUnk_0824F6A0);
            break;
        case 2:
        case 3:
            k2 = 6;
            a->state = k2;
            a->entered = 0;
            a->timer = 0;
            col = (u8 *)a + 0xf4;
            Collider_Init(col, 0, 0x2105, 0, 0x10, &gUnk_0824F6A0, &gUnk_0824F6A0);
            Collider_SetAttackProperties(col, ((struct AtkData *)*(u8 **)((u8 *)a + 0x150))->c,
                                         ((struct AtkData *)*(u8 **)((u8 *)a + 0x150))->d, 1,
                                         0x40000, 0);
            *(struct Pair *)(col + 0x1c) = gUnk_0824F6B0;
            break;
    }
    {
        register u8 f asm("r0") = Facing(flip);
        a->facing = f;
    }
    update = SceneEffect_UpdateActor;
    destroy = SceneEffect_DestroyActor;
    a->update = update;
    a->destroy = destroy;
}
