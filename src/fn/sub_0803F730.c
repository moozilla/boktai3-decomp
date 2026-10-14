#include "scene_arc.h"
#define GroundEffect_Initialize sub_0803F730
struct SceneGroundManager *sub_0821A2C4(u32);
struct SceneGroundManager *sub_0803F6F0(u32, u32);
void sub_08217DE0(void *, const void *, u32);
void sub_08217EC4(void *, s32, s32);
void sub_0803F64C(struct SceneGroundEffect *);
void sub_0803F650(struct SceneGroundEffect *);
void sub_0803F638(struct SceneGroundEffect *);
s32 sub_0803F5C4(struct SceneGroundManager *, struct SceneGroundEffect *);
s32 GroundEffect_Initialize(struct SceneGroundEffect *effect, const struct ScenePoolVec32 *position,
                            u32 kind)
{
    struct SceneGroundManager *manager = sub_0821A2C4(0xf0de);
    u32 zero;
    void *drawable;
    if (!manager) {
        manager = sub_0803F6F0(0, 0);
        if (!manager)
            return -1;
    }
    zero = 0;
    effect->active = zero;
    effect->position = position;
    effect->kind = kind;
    effect->counter = zero;
    effect->value4 = zero;
    effect->value6 = zero;
    drawable = &effect->flags;
    sub_08217DE0(drawable, manager->resource, 0x210);
    sub_08217EC4(drawable, -8, -8);
    {
        const u16 *resource = (const u16 *)manager->resource;
        effect->tile = (effect->width >> 3) * (effect->height >> 3) * 34 + resource[2];
    }
    effect->priority = 0xfc;
    effect->mode = 2;
    if (!effect->kind)
        effect->update = sub_0803F650;
    else {
        effect->update = sub_0803F64C;
        sub_0803F638(effect);
    }
    sub_0803F5C4(manager, effect);
    return 0;
}
