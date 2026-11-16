#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
s32 Mod(s32, s32);
void SceneActorAnimation_SpawnTrailAtPosition(void *, u32);
void SceneActorAnimation_RefreshTimedAttackCollider(void);
static inline u8 TakePrepared(struct ChaseActor *a)
{
    if (a->base.field2B2) {
        a->base.field2B2 = 0;
        return 1;
    }
    return 0;
}
void SceneActorAnimation_EmitDirectionalTrail(struct ChaseActor *a, s32 frame)
{
    union SceneActorRegistryPackedPosition p;
    if (TakePrepared(a))
        SceneActorAnimation_SelectDirectionalEntry((struct RoomActor *)a, 17);
    {
        struct DisplaySequenceState *sequence = (void *)((u8 *)a->base.drawable + 0x48);
        sequence->index = 0;
        sequence = (void *)((u8 *)a->base.drawable + 0x48);
        sequence->elapsed = 0;
    }
    if (Mod(frame, 4) == 0) {
        s32 direction = ((a->base.body.direction + 32) & 255) >> 6;
        p.position = a->base.body.position;
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
        SceneActorAnimation_SpawnTrailAtPosition(&p, a->base.paletteOffset);
    }
    if (frame > 30) {
        void *callback = SceneActorAnimation_RefreshTimedAttackCollider;
        u8 kind = 38;
        u8 *prepared = &a->base.field2B2;
        u32 zero = 0;
        *prepared = 1;
        a->base.field2B0 = zero;
        a->base.field2B5 = kind;
        a->base.target = callback;
        a->base.field2C8 = zero;
        {
            u32 *flags = &a->base.flags114;
            s32 mask = -2;
            *flags &= mask;
        }
        a->base.word104 = zero;
    }
}
