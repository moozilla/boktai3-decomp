#define SpriteGroup_CallCompletion FragmentBurst_RunCompletionScript
#include "global.h"
struct GroupPartSprite {
    u32 flags;
    u8 active;
    u8 unknown05[0x27];
};
struct SpriteGroup {
    struct GroupPartSprite sprites[4];
    u8 unknownB0[0xa4];
    u16 age;
    u8 active;
    s8 slot;
    u32 completionScript;
    u8 unknown15C[2];
    u8 pendingRelease;
};
struct SpriteGroupPool {
    u8 unknown00[0x38];
    u32 used;
};
void Sprite_Remove(void *);
void FragmentBurst_RunCompletionScript(struct SpriteGroup *);
void SpriteGroup_Release(struct SpriteGroupPool *pool, struct SpriteGroup *group)
{
    s32 i;
    if (group->pendingRelease) {
        pool->used &= ~(1 << group->slot);
        group->active = 0;
        i = 0;
        do {
            struct GroupPartSprite *sprite = &group->sprites[i];
            if (sprite->active)
                Sprite_Remove(sprite);
            i++;
        } while (i < 4);
        group->slot = -1;
        group->age = 0;
        FragmentBurst_RunCompletionScript(group);
        group->pendingRelease = 0;
    }
}
