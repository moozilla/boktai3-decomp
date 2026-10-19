#include "global.h"
#include "scene_actor_registry.h"

#ifdef NONMATCHING
// fuzzy: 98.9%
// foundry draft (worker best 98.9%); what differs is mostly register allocation and instruction order

struct AnimatedSpriteAnimation {
    u8 unknown0[6];
    u16 priority;
    u8 unknown8[4];
    void *shared;
    u8 unknown10[0x1c];
    void *resource;
    u16 frame;
    u8 unknown32[2];
};
struct AnimatedSpriteRecord {
    u32 flags;
    u8 unknown4[0x28];
    struct AnimatedSpriteAnimation animation;
};
struct AnimatedSpriteGroup {
    u8 unknown0[0x708];
    struct AnimatedSpriteRecord sprites[4];
    u8 unknown888[0x5bc];
    u16 state;
    u16 phase;
};
struct AnimatedSpriteView {
    u32 flags;
    u8 unknown4[0x2e];
    u16 priority;
    u8 unknown34[0x2c];
};
void SpriteGroup_InitAnimatedSprites(struct SceneActorRegistryActor *owner)
{
    struct AnimatedSpriteGroup *group = owner->context;
    u16 zero = 0;
    struct AnimatedSpriteAnimation *animation = &group->sprites[0].animation;
    struct AnimatedSpriteView *sprite = (void *)group->sprites;
    s32 n = 0;
    s32 i = 3;
    do {
        AnimData_Load(animation, 0x5291);
        Sprite_Init(sprite, animation, 0);
        sprite->priority = 0x13c;
        group->sprites[n].animation.shared = group->unknown888;
        group->sprites[n].animation.resource = ScriptProc_Start(0x922e, 0x3f72);
        sprite->flags = 0x201;
        animation->frame = zero;
        animation++;
        animation = (void *)((u8 *)animation + 0x2c);
        sprite++;
        n++;
    } while (--i >= 0);
    group->state = 0;
    group->phase = 2;
}
#else
INCLUDE_ASM("asm/nonmatching", SpriteGroup_InitAnimatedSprites);
#endif
