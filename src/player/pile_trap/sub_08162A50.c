#include "global.h"
#include "player/pile_trap/scene_marker.h"
#define TerrainOverlay_RegisterBody TerrainOverlay_QueueAddActor
struct SceneMarkerVector {
    u32 x : 16, y : 16, z : 16;
};
void PileTrap_OnCollision(void);
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Collider_SetAttackProperties(void *, s32, s32, s32, s32, s32);
void SceneBody_Init(struct SceneBody *, u32, struct SceneVec *, s32, s32, void *);
u32 TerrainOverlay_QueueAddActor(void *);
s32 SceneMarker_InitSprites(struct SceneMarker *p, u32 source)
{
    struct SceneVec position;
    struct SceneMarkerVector size, offset;
    void *collider;
    ZeroFill8(&position);
    collider = &p->collider;
    size.x = 128;
    size.y = 128;
    size.z = 128;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2001, 0, 1, &size, &offset);
    Collider_SetPosition(collider, &position, 0);
    Collider_SetCallback(collider, PileTrap_OnCollision, p);
    Collider_SetAttackProperties(collider, 100, 0, 0x100000, 1, 30);
    AnimData_Load(&p->primaryData, 0xdc7a);
    Sprite_Init(&p->primary, &p->primaryData, 1);
    SceneMarkerSprite_SetFrame(&p->primary, 0);
    p->primary.mode = 3;
    AnimData_Load(&p->secondaryData, 0xa680);
    Sprite_Init(&p->secondary, &p->secondaryData, 0x201);
    SceneMarkerSprite_SetFrame(&p->secondary, 11);
    p->secondary.mode = 1;
    AnimData_SelectPaletteBank(&p->secondaryData, 100);
    p->source = source;
    SceneBody_Init(&p->body, source, &p->primary.position, 0, 7, p);
    TerrainOverlay_RegisterBody(&p->body);
    p->body.flags |= 2;
    p->state = 0;
    p->colliderState = 0;
    return 0;
}
