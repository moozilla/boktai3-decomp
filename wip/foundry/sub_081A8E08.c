#define Vanargand_UpdatePlayerPositionRequest sub_081A8E08
#define gPlayer                               gUnk_02000580
#include "global.h"
struct Vanargand {
    u8 pad0[0x54];
    s16 position[4];
    u8 pad5C[0x9C - 0x5C];
    u32 flags;
    u8 padA0[0xAF - 0xA0];
    u8 finished, entered, pendingAnimation, animationEvent;
    u8 padB3[0x10C - 0xB3];
    u8 *controller;
    u8 pad110[0x18C - 0x110];
    u8 animation[0x60];
    u8 drawable[0x1AC];
    u8 pad398[0x7B8 - 0x398];
    u8 collision0[0x20], collision1[0x20], collision2[0x20];
};
extern void *gUnk_02000580;
void Vanargand_SetPendingCue(void *, u16);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, int, int, int);
u32 Player_GetCollisionData(void);
s32 Player_ApplyPositionRequest(void *, void *, s32, u32, s32, s32, s32, u32);
static inline s16 *vector(s16 *p, s16 x, s16 y, s16 z)
{
    s16 *q = p;
    *q = x;
    p[1] = y;
    p[2] = z;
    return p;
}
static inline void add(s16 *p, s16 *q)
{
    u16 x = q[0];
    u16 y = p[0];
    s16 *t = p;
    *t = x + y;
    p[1] = p[1] + q[1];
    p[2] = q[2] + p[2];
}
static inline void selectAnimation(struct Vanargand *p, void *animation, void *drawable, s32 mode,
                                   u32 zero)
{
    switch ((s32)p->controller[0x418]) {
        case 0:
            SceneActors_StartUnmirroredAnimation(p, animation, drawable, 12, mode, 0);
            CpuSet((void *)Player_GetCollisionData(), p->collision0, 0x04000008);
            break;
        case 1:
            SceneActors_StartUnmirroredAnimation(p, animation, drawable, 13, mode, 0);
            CpuSet((void *)Player_GetCollisionData(), p->collision1, 0x04000008);
            break;
        case 2:
            SceneActors_StartUnmirroredAnimation(p, animation, drawable, 14, mode, zero);
            CpuSet((void *)Player_GetCollisionData(), p->collision2, 0x04000008);
            break;
    }
}
void sub_081A8E08(struct Vanargand *p)
{
    struct Vanargand *p2 = p;
    s16 offset[4];
    u32 flags = 4;
    p->flags |= flags;
    Vanargand_SetPendingCue(p, 0);
    if (p->pendingAnimation) {
        u32 zero = 0;
        p->pendingAnimation = zero;
        add(vector(offset, 0x80, 0, 0x80), p->position);
        selectAnimation(p, p->animation, p->drawable, 2, zero);
    }
    if (p->animationEvent) {
        SceneActors_StartUnmirroredAnimation(p, p2->animation, p2->drawable, 3, 0, 2);
        Player_ApplyPositionRequest(gPlayer, (u8 *)p2 + 0x1AC, 0x80, 0x20, 0x78, 0, 0, 0);
        p->finished = 1;
    }
}
