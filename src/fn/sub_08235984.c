#include "global.h"
#include "engine.h"

/* Tentative family: task with one animation, two sprites and eight effects. */
struct SceneEffectGroup {
    u8 task[0x1c];
    u8 animation[0xcc];
    u8 sprites[2][0x2c];
    u8 unknown140[0x28];
    u8 effects[8][0x3c];
    u8 unknown348[4];
    u16 spriteMode;
};
void Sprite_Remove(void *);

s32 SceneEffectGroup_Remove(struct SceneEffectGroup *group)
{
    u8 *effect;
    s32 i;
    Anim_Remove(group->animation);
    effect = group->effects[0];
    for (i = 7; i >= 0; i--) {
        Effect_Remove(effect);
        effect += 0x3c;
    }
    if (group->spriteMode == 0) {
        effect = group->sprites[0];
        for (i = 1; i >= 0; i--) {
            Sprite_Remove(effect);
            effect += 0x2c;
        }
    }
    return 0;
}
