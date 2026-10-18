#include "global.h"
#define PlayerTransition_LoadAnimation sub_0815FFDC
struct TransitionResourceHeader { u32 words[8]; };
struct PlayerTransition {
    u8 unknown0[0x18];
    u8 *owner;
    struct TransitionResourceHeader resource;
    u8 animation[0x3a];
    u16 animationFlags;
    u8 unknown78[0xc];
    u32 attachment;
};
void *ScriptProc_Start(u16, u16);
s32 ResourceHeader_Relocate(void *, void *);
void sub_0821983C(void *, void *, int, int, int, int, int, void *);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void sub_0815FFDC(struct PlayerTransition *p)
{
    struct TransitionResourceHeader *resource = ScriptProc_Start(0xcb05, 0xde23);
    if (resource) {
        p->resource = *resource;
        ResourceHeader_Relocate(&p->resource, resource);
        sub_0821983C(p->animation, &p->resource, 0, 0x9000, 2, 0, 0x3c, p->owner + 0x30);
        Anim_SetKeyframes(p->animation, &p->resource, 0xe4, 1, 0);
        p->animationFlags = 0x33;
        p->attachment = *(u32 *)(p->owner + 0xd4);
    }
}
