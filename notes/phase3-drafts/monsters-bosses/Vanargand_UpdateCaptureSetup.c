#include "vanargand_pair_attack.h"
#include "player.h"

/* Copy the selected player palette and complete the capture position request. */
#define Vanargand_UpdateCaptureSetup sub_081A8E08
void Vanargand_SetPendingCue(void *, u16);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
u32 Player_GetCollisionData(void);
void CpuSet(const void *, void *, u32);
s32 Player_ApplyPositionRequest(struct Player *, struct SceneVec *, s32, u32, s32, s32, s32, u32);
static inline s32 SetPositionAndGetHeight(struct SceneVec *out, s32 x, s32 y, s32 z)
{
    s32 height = y;
    out->x = x;
    (&out->y)[0] = height;
    out->z = z;
    return height;
}
static inline void AddPosition(struct SceneVec *out, struct SceneVec *a,
                               struct SceneVec *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}

void Vanargand_UpdateCaptureSetup(struct VanargandPairAttack *p)
{
    struct VanargandPairAttack *q = p;
    struct SceneVec position;
    u32 mask = 4;
    p->flags |= mask;
    Vanargand_SetPendingCue(p, 0);
    if (p->pending) {
        void *animation;
        void *drawable;
        struct SceneVec *base;
        u32 two;
        s32 animationFlags;
        p->pending = 0;
        animationFlags = SetPositionAndGetHeight(&position, 128, 0, 128);
        base = &position;
        AddPosition(&position, &p->position, base);
        animation = &p->animation;
        drawable = p->drawable;
        two = 2;
        switch ((s32)p->player->actionMode) {
            case 0:
                SceneActors_StartUnmirroredAnimation(p, animation, drawable, 12, two,
                                                     animationFlags);
                CpuSet((const void *)Player_GetCollisionData(), p->palettes[5], 0x4000008);
                break;

            case 1:
                SceneActors_StartUnmirroredAnimation(p, animation, drawable, 13, two,
                                                     animationFlags);
                CpuSet((const void *)Player_GetCollisionData(), p->palettes[6], 0x4000008);
                break;

            case 2:
                SceneActors_StartUnmirroredAnimation(p, animation, drawable, 14, two, 0);
                CpuSet((const void *)Player_GetCollisionData(), p->palettes[7], 0x4000008);
                break;
        }
    }
    if (p->animationEvent) {
        SceneActors_StartUnmirroredAnimation(p, &q->animation, q->drawable, 3, 0, 2);
        Player_ApplyPositionRequest(gPlayers[0], &q->animation.position, 128, 32, 120, 0, 0, 0);
        p->finished = 1;
    }
}
