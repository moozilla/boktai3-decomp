#include "global.h"
typedef struct ActorAnimation {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x4a];
    u32 flags;
    u8 padA0[8];
    u8 facing, facing8;
    u8 padAA[7];
    u8 changed;
    u8 padB2;
    u8 locked;
    u8 padB4[0x18];
    u8 requestedAngle;
    u8 padCD[0x37];
    u8 (*tryUpdate)(void *);
    void (*reset)(void *);
    u8 pad10C[8];
    s8 savedAngle;
    u8 pad115[0x22e];
    u8 variant;
    u16 scale, minScale, maxScale;
    u8 pad34A[0x62];
    u32 mode;
} ActorAnimation;
static inline void setFacing(ActorAnimation *p, int angle)
{
    int shifted;
    u32 value;
    p->angle = angle;
    shifted = p->angle + 0x60;
    shifted >>= 6;
    value = 3;
    value &= shifted;
    p->facing = value;
    shifted = p->angle + 0x70;
    shifted >>= 5;
    value = 7;
    value &= shifted;
    p->facing8 = value;
}

void SceneBody_Init(void *, int, int, int, int, void *);
void SceneBody_SetPositionOffset(void *, void *);
void SceneMessages_RegisterReceiver(void *, int, int);
int SceneActors_Initialize(u8 *p, int id, int type, int arg, int facingArg)
{
    int firstId = id;
    int facing = facingArg;
    struct {
        s16 x, y, z;
    } v;
    u8 *dst = p + 0x9b;
    int zero = 0;
    *dst = type;
    SceneBody_Init(p + 0x48, firstId, arg, (u8)facing, 5, p);
    v.x = 0;
    v.y = 256;
    v.z = 0;
    SceneBody_SetPositionOffset(p + 0x48, &v);
    setFacing((ActorAnimation *)p, facing);
    p[0x114] = facing;
    *(int *)(p + 0x16c) = zero;
    *(int *)(p + 0x158) = zero;
    *(int *)(p + 0x170) = zero;
    SceneMessages_RegisterReceiver(p + 0x118, id, 4);
    return 0;
}
