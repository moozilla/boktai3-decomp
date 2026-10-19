#include "global.h"
#define TetherActor_InitRequestIndicator sub_081C0390

struct ResourceHeader8 {
    u32 words[8];
};

struct TetherRequestIndicator {
    u8 task[0x18];
    u8 anim[8];
    u32 flags;
    u8 unk24[0x54];
    struct ResourceHeader8 resource;
    u8 unk98[8];
    u16 unkA0;
    u16 unkA2;
    u8 unkA4[4];
    void *actor;
};

void *ScriptProc_Start(u16 key, u16 context);
s32 ResourceHeader_Relocate(void *dst, void *src);
void sub_0821983C(void *, void *, int, int, int, int, int, void *);
s32 DirectionMarker_GetAnimation(s32 base, s32 group);
void Anim_SetKeyframes(void *anim, void *data, u32 index, u32 mode, u32 flags);
void *SceneActors_GetBySlot(u32);

s32 TetherActor_InitRequestIndicator(struct TetherRequestIndicator *p)
{
    struct ResourceHeader8 *resource;

    resource = ScriptProc_Start(0xCB05, 0xF771);
    p->resource = *resource;
    ResourceHeader_Relocate(&p->resource, resource);
    sub_0821983C(p->anim, &p->resource, 0, 0x10, 0, 1, 0x3c, 0);
    Anim_SetKeyframes(p->anim, &p->resource, (u16)DirectionMarker_GetAnimation(0, 0), 0, 0);
    p->flags |= 1;
    p->actor = SceneActors_GetBySlot(1);
    p->unkA2 = 0;
    p->unkA0 = 0xFFFF;
    return 0;
}
