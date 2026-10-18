#define SceneActorAnimation_EmitDirectionalTrail       sub_080E17D8
#define SceneActorAnimation_RefreshTimedAttackCollider sub_080E18F4
#define SceneActorAnimation_SpawnTrailAtPosition       sub_080DF908
#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
s32 Mod(s32, s32);
void sub_080DF908(void *, u32);
void sub_080E18F4(void);
static inline u8 TakePrepared(struct SceneActorRegistryResetView *a)
{
    if (a->field2B2) {
        a->field2B2 = 0;
        return 1;
    }
    return 0;
}
void sub_080E17D8(struct SceneActorRegistryResetView *a, s32 frame)
{
    union SceneActorRegistryPackedPosition p;
    if (TakePrepared(a))
        SceneActorAnimation_SelectDirectionalEntry((struct SceneActorAnimationView *)a, 17);
    {
        struct DisplaySequenceState *sequence = (void *)((u8 *)a->drawable + 0x48);
        sequence->index = 0;
        sequence = (void *)((u8 *)a->drawable + 0x48);
        sequence->elapsed = 0;
    }
    if (Mod(frame, 4) == 0) {
        s32 direction = ((a->direction + 32) & 255) >> 6;
        p.position = a->position;
        switch (direction) {
            case 0:
                p.packed.x += 128;
                break;
            case 1:
                p.packed.z += 128;
                break;
            case 2:
                p.packed.x -= 160;
                break;
            case 3:
                p.packed.z -= 128;
                break;
        }
        sub_080DF908(&p, a->paletteOffset);
    }
    if (frame > 30) {
        void *callback = sub_080E18F4;
        u8 kind = 38;
        u8 *prepared = &a->field2B2;
        u32 zero = 0;
        *prepared = 1;
        a->field2B0 = zero;
        a->field2B5 = kind;
        a->target = callback;
        a->field2C8 = zero;
        {
            u32 *flags = &a->flags114;
            s32 mask = -2;
            *flags &= mask;
        }
        a->word104 = zero;
    }
}
