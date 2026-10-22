#include "global.h"
// MATCHING_CONCESSION: register-pinning

#ifdef NONMATCHING
// fuzzy: 92.9%
#define ActorController_SetAnimation sub_08182454
// Remaining: side-constant scratch registers and final mode/flags argument scheduling.
#define Armor_SetAnimation       ActorController_SetAnimation
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
    register s32 variant asm("sb");
    s32 kind;
    register u16 actorFlags asm("r4") = s->flags;
    s32 mode;
    side = 1;
    if (actorFlags & 1)
        side = 0;
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
    Anim_SetKeyframes(s->animation, s->animationData, gUnk_0824E920[variant][side][index], mode,
                      (u16)flags);
    s->animationSpeed = 0x40;
    s->flags &= ~0x20;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08182454);
#endif
