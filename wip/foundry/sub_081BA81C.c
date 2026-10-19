#define Registry3F74_InitAnimations sub_081BA81C
#define Anim_InitSlot sub_0821983C
#include "global.h"

struct Registry3F74Animation { u8 unknown[0x1A]; u8 mode; u8 tail[0x45]; };
struct Registry3F74AnimData { u32 words[8]; };
struct Registry3F74Object {
    u8 unknown[0x20];
    struct Registry3F74Animation animations[5];
    struct Registry3F74AnimData data;
};
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 Anim_Step(void *, void *);
s32 ResourceHeader_Relocate(void *, void *);
void *ScriptProc_Start(u16, u16);
void sub_0821983C(void *, void *, int, int, int, int, int, void *);
void sub_081BA81C(struct Registry3F74Object *p)
{
    struct Registry3F74AnimData *resource = ScriptProc_Start(0xCB05, 0xA2DB);
    struct Registry3F74Animation *anim;
    s32 i;
    if (resource) {
        p->data = *resource;
        ResourceHeader_Relocate(&p->data, resource);
        anim = &p->animations[0];
        i = 4;
        do {
            sub_0821983C(anim, &p->data, 0, 0x10, 2, 0, 0x3C, 0);
            anim->mode = 3;
            anim++;
        } while (--i >= 0);
        Anim_LoadKeyframe(&p->animations[2], &p->data, 0x26, 0);
        p->animations[2].mode = 1;
        Anim_LoadKeyframe(&p->animations[0], &p->data, 0x27, 0);
        Anim_LoadKeyframe(&p->animations[1], &p->data, 0x2B, 0);
        p->animations[1].mode = 2;
        Anim_LoadKeyframe(&p->animations[3], &p->data, 0, 0);
        p->animations[3].mode = 2;
        anim = &p->animations[4];
        Anim_SetKeyframes(anim, &p->data, 4, 1, 0);
        Anim_Step(anim, &p->data);
    }
}
