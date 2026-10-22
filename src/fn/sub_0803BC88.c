#include "scene_pool.h"
#ifdef NONMATCHING
// fuzzy: 96.0%
struct WirelessSceneActor {
    u8 unknown0[4];
    u32 terrainFlags, previousTerrainFlags;
    u8 unknownC[4];
    struct ScenePoolVec32 position;
    s32 motion[3];
    u8 unknown2c[0xc];
    s32 heightA, heightB;
    u32 flags40;
    u8 colorEffect[4];
    u8 unknown48[3];
    u8 paletteIndex;
    u8 unknown4c[0xc];
    u16 timerA, timerB;
    s32 parameter;
    u8 unknown60[8];
    u8 preview[0x58];
    s32 forwardBias;
    u8 unknownc4[0x468];
    u8 contactIndex[0x40];
    u32 motionFlags;
    u8 unknown570[0x3c];
    s32 forwardSpeed, verticalSpeed, lateralSpeed;
    u8 unknown5b8[4];
    s32 previousX;
    u8 unknown5c0[4];
    s32 previousZ;
    u8 unknown5c8[4];
    s32 impulse[4];
    s32 offsetX;
    u8 unknown5e0[0xc];
    s8 impulseFrames;
    u8 unknown5ed[3];
    u16 heading;
    u8 unknown5f2[0xe];
    u8 model[0x250];
    s8 marker;
    u8 unknown851[3];
    u32 index;
    u8 unknown858[8];
    u32 controlFlags;
};
void ScenePool_InitActorAppearance(void *, struct ScenePoolEntry *, u32, u32);
void Terrain_ResolveActorMotion(void *);
void Character_GetHeightAdjustedPosition(void *, struct Vec16 *);
void SpatialIndex_SetPosition(void *, s32, s32, s32);
void Collider_SetContactParameter(void *, s32);
void ScenePool_ResetProximityContact(void *);
void CharacterModel_ClearAnimationFlags(void *, u32);
void ScenePool_UpdateActorDrawablePosition(void *);
void CharacterPreview_InitializePalette(void *, void *, u8);
void Character_SetLowResourceColorEffect(void *, void *);
void ActorMarkerPool_ShowMarker(s32);
void Actor_ClampMovementSpeed(void *);

struct Vec16 {
    s16 x, y, z;
};

static inline s32 Scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

void sub_0803BC88(struct WirelessSceneActor *a, struct ScenePoolEntry *e, u32 index)
{
    s32 speed;
    s32 dx, dz;
    struct Vec16 pos;
    u32 above, below;
    s32 lim;
    s32 *pprevX, *pprevZ;
    u16 *ph;
    u32 *motion;
    u8 *contact;
    s32 *vert;
    s32 *ox, *i1, *i2;

    ScenePool_InitActorAppearance(a, e, e->duration, 100);
    a->timerA = 0;
    a->timerB = 0;
    a->parameter = e->duration;
    a->position = e->position;
    speed = a->forwardSpeed + a->forwardBias;
    if (speed > 0xF000)
        speed = 0xF000;
    a->verticalSpeed += 0x200;
    vert = &a->verticalSpeed;
    if (a->impulseFrames > 0) {
        a->offsetX += a->impulse[0];
        a->impulse[0] += Div(-a->impulse[0], a->impulseFrames);
        a->impulse[1] += Div(-a->impulse[1], a->impulseFrames);
        a->impulse[2] += Div(-a->impulse[2], a->impulseFrames);
        a->impulseFrames--;
        ox = &a->offsetX;
        i1 = &a->impulse[1];
        i2 = &a->impulse[2];
    } else {
        Memory_Clear(a->impulse, 0x10);
        ox = &a->offsetX;
        i1 = &a->impulse[1];
        i2 = &a->impulse[2];
    }
    Actor_ClampMovementSpeed(a);
    a->motion[0] = *ox + speed - a->previousX;
    a->motion[1] = *vert + *i1;
    a->motion[2] = a->lateralSpeed + *i2;
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
    above = (u32) ~(a->heightB >> 12) >> 31;
    below = (u32) ~(a->heightA >> 12) >> 31;
    if (a->terrainFlags != a->previousTerrainFlags && (a->terrainFlags & 0x1000) &&
        !(a->previousTerrainFlags & 0xF000) && below <= 1 && above == 1) {
        s32 v = ((a->forwardSpeed >> 10) + (a->forwardBias >> 8)) >> 1;
        if (v < 4)
            v = 4;
        if (v > 0x12)
            v = 0x12;
        *vert -= v << 9;
        a->flags40 &= ~1;
    }
    {
        Character_GetHeightAdjustedPosition(a, &pos);
        SpatialIndex_SetPosition(&a->motionFlags, pos.x, pos.y, pos.z);
        motion = &a->motionFlags;
        if ((s16)*ph < 0) {
            SpatialIndex_SetPosition(a->contactIndex, pos.x, pos.y, pos.z);
            contact = a->contactIndex;
        } else {
            struct Vec16 off;
            struct Vec16 *o = &off;
            s32 angle = (s16)*ph;
            s32 radius = 20;
            o->x = Scale(gUnk_086149C4[(angle + 0x40) & 0xFF] * radius);
            o->y = 0;
            o->z = Scale(gUnk_086149C4[angle & 0xFF] * radius);
            SpatialIndex_SetPosition(a->contactIndex, pos.x + off.x, pos.y, pos.z + off.z);
            contact = a->contactIndex;
        }
    }
    *motion &= ~1;
    Collider_SetContactParameter(contact, e->parameter);
    ScenePool_ResetProximityContact(a);
    if (e->kind == 3)
        a->controlFlags &= ~1;
    else
        a->controlFlags |= 1;
    CharacterModel_ClearAnimationFlags(a->model, 1);
    ScenePool_UpdateActorDrawablePosition(a);
    CharacterPreview_InitializePalette(a->preview, a->model, a->paletteIndex);
    Character_SetLowResourceColorEffect(a->colorEffect, a->unknown48);
    ActorMarkerPool_ShowMarker(a->marker);
    a->index = index;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0803BC88);
#endif
