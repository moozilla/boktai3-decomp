#define PanelKeyframeSprite_Create sub_08112D6C
#include "global.h"
struct PanelKeyframeSprite {
    u8 resources[0x20];
    u8 anim[0x60];
};
struct PanelKeyframeOwner {
    struct PanelKeyframeSprite *sprite;
    u8 pad4[0x17];
    u8 state;
};
void *Heap_Alloc(u32);
void Memory_Clear(void *, u32);
void *ScriptProc_Start(u16, u16);
s32 ResourceHeader_Relocate(void *, void *);
void sub_0821983C(void *, void *, int, int, int, int, int, void *);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 Anim_Step(void *, void *);
static inline void ClearPosition(void *src, void *dst)
{
    CpuSet(src, dst, 0x05000002);
}
s32 sub_08112D6C(struct PanelKeyframeOwner *owner, u32 key, u32 index, u32 flags, u32 mode)
{
    struct PanelKeyframeSprite *p;
    struct {
        u32 zero;
        s32 position[2];
    } local;
    void *pos;
    u8 *anim;
    u32 spriteFlags;
    u32 frame;
    owner->sprite = Heap_Alloc(0x80);
    if (!owner->sprite)
        return -1;
    Memory_Clear(owner->sprite, 0x80);
    owner->state = 1;
    p = owner->sprite;
    pos = local.position;
    local.zero = 0;
    ClearPosition(&local.zero, pos);
    ResourceHeader_Relocate(p, ScriptProc_Start(0xCB05, (u16)key));
    spriteFlags = flags | 0x11;
    anim = p->anim;
    frame = (u16)index;
    sub_0821983C(anim, p, frame, spriteFlags, (u8)mode, 2, 0x3C, pos);
    Anim_SetKeyframes(anim, p, frame, 2, 0);
    Anim_Step(anim, p);
    return 1;
}
