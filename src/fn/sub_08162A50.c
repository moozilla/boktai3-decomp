#include "global.h"
#include "scene_object.h"
// MATCHING_CONCESSION: register-pinning
#define TerrainOverlay_RegisterBody TerrainOverlay_QueueAddActor
struct SceneMarkerVector {
    u32 x : 16, y : 16, z : 16;
};
struct SceneMarkerSprite {
    u8 unknown0[7];
    u8 mode;
    u8 unknown8[8];
    u16 frame;
    u8 unknown12[0xa];
    struct SceneVec position;
    u8 unknown24[8];
};
struct SceneMarker {
    u8 task[0x18];
    struct SceneBody body;
    u32 source;
    u8 collider[0x54];
    u32 colliderState;
    u8 secondaryData[0x1c];
    u8 primaryData[0x1c];
    struct SceneMarkerSprite primary;
    struct SceneMarkerSprite secondary;
    u32 state;
};
void PlayerBarrier_OnCollision(void);
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void SceneBody_Init(struct SceneBody *, u32, struct SceneVec *, s32, s32, void *);
u32 TerrainOverlay_QueueAddActor(void *);
static inline void SetFrame(struct SceneMarkerSprite *sprite, u16 frame)
{
    sprite->frame = frame;
}
static inline void CpuSetFill(void *dest, u32 control, const void *src)
{
    CpuSet(src, dest, control);
}
s32 SceneMarker_InitSprites(struct SceneMarker *p, u32 source)
{
    struct {
        u32 zero;
        struct SceneVec position;
    } local;
    struct SceneMarkerVector size, offset;
    register struct SceneVec *positionPtr asm("r9") = &local.position;
    void *collider;
    local.zero = 0;
    {
        u32 *src = &local.zero;
        asm("" : "+r"(src));
        CpuSetFill(positionPtr, 0x5000002, src);
    }
    collider = p->collider;
    size.x = 128;
    size.y = 128;
    size.z = 128;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2001, 0, 1, &size, &offset);
    Collider_SetPosition(collider, positionPtr, 0);
    Collider_SetCallback(collider, PlayerBarrier_OnCollision, p);
    Collider_SetAttackProperties(collider, 100, 0, 0x100000, 1, 30);
    AnimData_Load(p->primaryData, 0xdc7a);
    Sprite_Init(&p->primary, p->primaryData, 1);
    SetFrame(&p->primary, 0);
    p->primary.mode = 3;
    AnimData_Load(p->secondaryData, 0xa680);
    Sprite_Init(&p->secondary, p->secondaryData, 0x201);
    SetFrame(&p->secondary, 11);
    p->secondary.mode = 1;
    AnimData_SelectPaletteBank(p->secondaryData, 100);
    p->source = source;
    SceneBody_Init(&p->body, source, &p->primary.position, 0, 7, p);
    TerrainOverlay_RegisterBody(&p->body);
    p->body.flags |= 2;
    p->state = 0;
    p->colliderState = 0;
    return 0;
}
