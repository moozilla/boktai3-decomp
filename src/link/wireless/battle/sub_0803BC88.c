#include "link/wireless/scene_pool_collision.h"
#include "scene/collision/spatial_index.h"
#include "engine/math/scene_vec_polar.h"
void ScenePool_InitActorAppearance(void *, struct ScenePoolEntry *, u32, u32);
void Terrain_ResolveActorMotion(void *);
void Character_GetHeightAdjustedPosition(void *, struct ContactPosition *);
void ScenePool_ResetProximityContact(void *);
void CharacterModel_ClearAnimationFlags(void *, u32);
void ScenePool_UpdateActorDrawablePosition(void *);
void CharacterPreview_InitializePalette(void *, void *, u8);
void Character_SetLowResourceColorEffect(void *, void *);
void ActorMarkerPool_ShowMarker(s32);
void Actor_ClampMovementSpeed(void *);

static inline s32 Scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

void sub_0803BC88(struct ScenePoolActor *a, struct ScenePoolEntry *e, u32 index)
{
    s32 speed;
    s32 dx, dz;
    struct ContactPosition pos;
    u32 above, below;
    s32 lim;
    s32 mask;
    s32 heading;
    u32 h;
    s32 frames;
    s32 *pprevX, *pprevZ;
    u16 *ph;
    u32 *motion;
    struct SpatialEntry *contact;
    s32 *vert;
    s32 *ox, *i1, *i2;

    ScenePool_InitActorAppearance(a, e, e->duration, 100);
    {
        s32 zero = 0;
        u32 duration = e->duration;
        a->timerA = zero;
        a->timerB = zero;
        a->currentAmount = duration;
    }
    a->position = e->position;
    speed = a->forwardSpeed + a->forwardBias;
    if (speed > 0xF000)
        speed = 0xF000;
    a->verticalSpeed += 0x200;
    frames = a->motionLock;
    vert = &a->verticalSpeed;
    if (frames > 0) {
        a->steering += a->impulse[0];
        a->impulse[0] += Div(-a->impulse[0], a->motionLock);
        a->impulse[1] += Div(-a->impulse[1], a->motionLock);
        a->impulse[2] += Div(-a->impulse[2], a->motionLock);
        a->motionLock--;
        ox = &a->steering;
        i1 = &a->impulse[1];
        i2 = &a->impulse[2];
    } else {
        Memory_Clear(a->impulse, 0x10);
        ox = &a->steering;
        i1 = &a->impulse[1];
        i2 = &a->impulse[2];
    }
    Actor_ClampMovementSpeed(a);
    a->motionX = *ox + speed - a->previousX;
    a->motionY = *vert + *i1;
    a->motionDisplacement = a->lateralSpeed + *i2;
    dx = (*ox - a->previousX) >> 12;
    dz = (a->position.z - a->previousZ) >> 12;
    lim = 0x3000;
    pprevX = &a->previousX;
    pprevZ = &a->previousZ;
    if (speed > lim)
        dx = 0x80;
    if (dx == 0 && dz == 0) {
        a->heading = 0xFFFF;
        ph = &a->heading;
    } else {
        a->heading = (Math_Atan2(dx, dz) >> 5) << 5;
        ph = &a->heading;
    }
    *pprevX = *ox;
    *pprevZ = a->position.z;
    Terrain_ResolveActorMotion(a);
    above = (u32) ~(a->terrainHeightB >> 12) >> 31;
    below = (u32) ~(a->restrictionMetric >> 12) >> 31;
    if (a->unknown4 != a->kind && (a->unknown4 & 0x1000) && !(a->kind & 0xF000) && below <= 1 &&
        above == 1) {
        s32 v = ((a->forwardSpeed >> 10) + (a->forwardBias >> 8)) >> 1;
        if (v < 4)
            v = 4;
        if (v > 0x12)
            v = 0x12;
        *vert -= v << 9;
        a->movementFlags &= ~1;
    }
    {
        Character_GetHeightAdjustedPosition(a, &pos);
        SpatialIndex_SetPosition(ScenePool_MotionCollider(a), pos.x, pos.y, pos.z);
        h = *ph;
        heading = (s16)h;
        motion = &ScenePool_MotionCollider(a)->flags;
        if (heading < 0) {
            SpatialIndex_SetPosition(&a->hitCollider, pos.x, pos.y, pos.z);
            contact = &a->hitCollider;
        } else {
            struct SceneVec off;
            SceneVec_SetPolarXZ(&off, (s16)*ph, 20);
            SpatialIndex_SetPosition(&a->hitCollider, pos.x + off.x, pos.y, pos.z + off.z);
            contact = &a->hitCollider;
        }
    }
    mask = ~1;
    *motion &= mask;
    Collider_SetContactParameter(contact, e->parameter);
    ScenePool_ResetProximityContact(a);
    if (e->kind == 3) {
        a->proximityCollider.flags &= mask;
    } else {
        u32 *flags = &a->proximityCollider.flags;
        u32 bit = 1;
        *flags = bit | *flags;
    }
    CharacterModel_ClearAnimationFlags(a->model, 1);
    ScenePool_UpdateActorDrawablePosition(a);
    CharacterPreview_InitializePalette(&a->unknown68, a->model, a->paletteState);
    Character_SetLowResourceColorEffect(a->colorEffect, &a->bodyAnimation);
    ActorMarkerPool_ShowMarker(a->marker);
    a->entryIndex = index;
}
