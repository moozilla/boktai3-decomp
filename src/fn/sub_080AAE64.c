#include "global.h"
/* TentacleActor_InitSegmentColliders: Sets up body and attack colliders for every tentacle segment */
#define TentacleActor_InitSegmentColliders sub_080AAE64

struct TentaclePacked {
    s32 x : 16, y : 16, z : 16, pad : 16;
};

void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetBodyProperties(void *, u32, u32, u32);
void Collider_SetPosition(void *, void *, s32);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void TentacleActor_ReactToContact(void);
void TentacleActor_OnSegmentAttackCollision(void);

struct Body080AAE64 {
    u8 pad0[0x3d4];
    u8 sprites[4][0x128];
    u8 pad874[0x8da - 0x874];
    s16 count;
};

static inline void SetAttack(void *collider, u8 *p, s32 variant)
{
    u8 *q = p + 0xb4 + variant;
    u32 a = *(u16 *)q;
    u32 b;
    u32 c;
    u32 d;
    q += 8;
    b = *(u16 *)q;
    q += variant ? 0x12 : 0x10;
    c = *(u32 *)q;
    q -= variant ? 0xa : 8;
    d = *(u16 *)q;
    Collider_SetAttackProperties(collider, a, b, 0, c, d);
}

void sub_080AAE64(u8 *p)
{
    struct Body080AAE64 *g = *(struct Body080AAE64 **)(p + 0x3d0);
    struct TentaclePacked size, offset;
    s32 i;
    size.x = 64;
    size.y = 128;
    size.z = 64;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    for (i = 0; i < g->count; i++) {
        u8 *base = (u8 *)g;
        void *owner = base + 0x3d4 + i * 0x128;
        void *collider = base + 0x470 + i * 0x128;
        void *position = base + 0x3f0 + i * 0x128;
        Collider_Init(collider, *(u32 *)(p + 0x50), 0x4109, 0, 16, &size, &offset);
        Collider_SetBodyProperties(collider, *(u8 *)(p + 0x9e), 0, 0);
        Collider_SetPosition(collider, position, 0);
        Collider_SetCallback(collider, TentacleActor_ReactToContact, owner);
    }
    {
        u32 zero = 0;
        u32 width = 64;
        ((u16 *)&size)[0] = width;
        ((u16 *)&size)[1] = 100;
        ((u16 *)&size)[2] = width;
        ((u16 *)&offset)[0] = zero;
        ((u16 *)&offset)[1] = 128;
        ((u16 *)&offset)[2] = zero;
        i = zero;
    }
    for (i = 0; i < g->count; i++) {
        u8 *base = (u8 *)g;
        void *owner = base + 0x3d4 + i * 0x128;
        void *collider = base + 0x41c + i * 0x128;
        void *position = base + 0x3f0 + i * 0x128;
        Collider_Init(collider, *(u32 *)(p + 0x50), 0x2401, 0, 16, &size, &offset);
        SetAttack(collider, p, 0);
        Collider_SetPosition(collider, position, 0);
        Collider_SetCallback(collider, TentacleActor_OnSegmentAttackCollision, owner);
    }
}
