#include "global.h"
// MATCHING_CONCESSION: register-pinning

#define gArmorAnimationVariants  gUnk_0824EB60
#define gArmorAnimationKeyframes gUnk_0824E920
struct ArmorActor {
    u8 pad0[0x94];
    u8 animationData[0x20];
    u8 animation[8];
    u32 renderFlags;
    u8 padC0[4];
    u16 animationSpeed;
    u8 padC6[0xA6C];
    u16 flags;
    u8 padB34[4];
    u8 variant;
};
struct ArmorAnimationRequest {
    u16 index;
    u8 flag;
    u8 mode;
};
extern const u8 gUnk_0824EB60[];
extern const u16 gUnk_0824E920[][2][36];
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void ActorController_SetAnimation(struct ArmorActor *s, struct ArmorAnimationRequest *request,
                                  u32 flags)
{
    u32 index = request->index;
    register u32 side asm("ip");
    s32 variant;
    s32 kind;
    register u16 actorFlags asm("r4") = s->flags;
    s32 mode;
    {
        u32 test = actorFlags & 1;
        u32 one = 1;
        side = one;
        if (test) {
            u32 zero = 0;
            side = zero;
        }
    }
    kind = s->variant;
    if (actorFlags & 0x200)
        kind += 4;
    variant = gUnk_0824EB60[kind];
    switch ((s32)request->mode) {
        case 0:
        default:
            mode = 2;
            break;
        case 1:
            mode = 1;
            break;
        case 2:
            mode = 0;
            break;
    }
    if (request->flag)
        s->renderFlags |= 4;
    else
        s->renderFlags &= ~4;
    {
        void *animation = s->animation;
        void *data = s->animationData;
        u32 keyframe = gUnk_0824E920[variant][side][index];
        u32 callMode = mode;
        u32 wideFlags;
        u32 callFlags;
        asm("" : "+r"(animation), "+r"(data), "+r"(keyframe), "+r"(callMode));
        wideFlags = flags;
        callFlags = (u16)wideFlags;
        Anim_SetKeyframes(animation, data, keyframe, callMode, callFlags);
    }
    s->animationSpeed = 0x40;
    {
        u32 offset = 0xB32;
        *(u16 *)((u8 *)s + offset) &= ~0x20;
    }
}
