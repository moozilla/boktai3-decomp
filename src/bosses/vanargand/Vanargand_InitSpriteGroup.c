#include "global.h"

struct VanargandGroupSprite {
    u32 flags;
    u8 unknown4[3];
    u8 mode;
    u8 unknown8[8];
    u16 order;
    u8 unknown12[10];
    s16 position[4];
    u8 unknown24[8];
};

struct VanargandGroupAnimation {
    u8 unknown0[6];
    u16 field6;
    u8 unknown8[4];
    void *anchor;
    u8 unknown10[12];
};

struct VanargandSpriteGroup {
    struct VanargandGroupSprite sprites[6];
    struct VanargandGroupAnimation animation;
    void *field124;
    void *field128;
};

s32 AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);

void Vanargand_InitSpriteGroup(u8 *owner, struct VanargandSpriteGroup *group, void *arg2,
                               void *arg3)
{
    struct VanargandGroupAnimation *animation;
    struct VanargandGroupSprite *sprite;
    s32 i;
    u32 zero;
    void *position;

    group->field124 = arg2;
    group->field128 = arg3;
    animation = &group->animation;
    AnimData_Load(animation, 0x89CB);
    animation->field6 = 0x6F;
    animation->anchor = owner + 0x718;
    sprite = group->sprites;
    for (i = 0; i <= 5; i++) {
        Sprite_Init(sprite, &group->animation, 0);
        sprite->order = i + 0x18;
        position = sprite->position;
        zero = 0;
        CpuSet(&zero, position, 0x05000002);
        sprite->flags |= 1;
        sprite->mode = 2;
        sprite++;
    }
}
