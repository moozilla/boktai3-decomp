#include "scene_pool.h"
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

/* Layout of the actor as this function sees it (offsets from the ROM). */

// confidence=low: wireless scene actor setup; initializes model, style and drain state, steps the impulse decay, derives heading and a terrain bounce, then clears the index.

/* Layout of the actor as this function sees it (offsets from the ROM). */

// confidence=low: wireless scene actor setup; initializes model, style and drain state, steps the impulse decay, derives heading and a terrain bounce, then clears the index.
void WirelessObj_InitCollisionVolumes(void *);
void ActorSprite_InitPartDrawings(void *, void *, u32);
void CharacterModel_SetAnimationFlags(void *, int);
s32 Div(s32, s32);
void Modifier_InitDrainState(void *, void *, u16, u8);
void ActorStats_ConfigureScaledParameters(void *, int, int, int, int, int, int);
void CharacterModel_SetPrimaryAppearance(void *, void *);
void ActorMarkerPool_HideMarker(int);
void SceneActorStyle_Initialize(void *, int, int, int, int, int, int, u32, u32);
void ActorAction_InitializeMode(void *, void *, int);
void CharacterPreview_InitializePalette(void *, void *, u8);

/* Layout of the actor as this function sees it (offsets from the ROM). */
struct Sub0803CEE8Actor {
    u8 unk00[4];
    u32 terrainFlags;
    u32 previousTerrainFlags;
    u8 unk0C[4];
    struct ScenePoolVec32 position;
    s32 motion[3];
    u8 unk2C[0xC];
    s32 heightA;
    s32 heightB;
    u32 flags40;
    u8 unk44[4];
    u8 styleBytes[0x20];
    u8 unk68[0x58];
    s32 forwardBias;
    u8 unkC4[0x4E8];
    s32 forwardSpeed;
    s32 verticalSpeed;
    s32 lateralSpeed;
    u8 unk5B8[4];
    s32 previousX;
    u8 unk5C0[4];
    s32 previousZ;
    u8 unk5C8[4];
    s32 impulse[4];
    s32 offsetX;
    u8 unk5E0[0xC];
    s8 impulseFrames;
    u8 unk5ED[3];
    u16 heading;
    u8 unk5F2[2];
    void *selfPtr;
    u8 unk5F8[8];
    u8 model[0x250];
    s8 marker;
    u8 unk851[3];
    s32 index;
};

#ifdef NONMATCHING
// fuzzy: 58.1%
// scope shape 41 (differences once low registers are set aside)
// confidence=low: wireless scene actor setup; initializes model, style and drain state, steps the impulse decay, derives heading and a terrain bounce, then clears the index.
int sub_0803CEE8(void *unused, struct Sub0803CEE8Actor *a)
{
    s32 speed;
    s32 dx, dz;
    s32 *vert;
    s32 *ox;
    s32 *i1;
    s32 *i2;
    s32 v;
    u32 above, below;

    WirelessObj_InitCollisionVolumes(a);
    a->selfPtr = a;
    {
        u8 *p = a->styleBytes;
        u8 c1 = 0xC;
        u8 c2 = 0x12;
        u8 c3 = 6;
        u8 c4 = 0;
        *p = c1;
        p += 2;
        *p = c2;
        p -= 1;
        *p = c3;
        p += 3;
        *p = c4;
    }
    ActorSprite_InitPartDrawings(a, a->model, 0x3D99);
    CharacterModel_SetAnimationFlags(a->model, 1);

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
    } else {
        Memory_Clear(a->impulse, 0x10);
    }
    ox = &a->offsetX;
    i1 = &a->impulse[1];
    i2 = &a->impulse[2];

    Actor_ClampMovementSpeed(a);
    a->motion[0] = *ox + speed - a->previousX;
    a->motion[1] = *vert + *i1;
    a->motion[2] = a->lateralSpeed + *i2;

    dx = (*ox - a->previousX) >> 12;
    dz = (a->position.z - a->previousZ) >> 12;
    if (speed > 0x3000)
        dx = 0x80;
    if (dx == 0 && dz == 0) {
        a->heading = 0xFFFF;
    } else {
        a->heading = (Math_Atan2(dx, dz) >> 5) << 5;
    }
    a->previousX = *ox;
    a->previousZ = a->position.z;

    Terrain_ResolveActorMotion(a);
    above = (u32) ~(a->heightB >> 12) >> 31;
    below = (u32) ~(a->heightA >> 12) >> 31;
    if (a->terrainFlags != a->previousTerrainFlags && (a->terrainFlags & 0x1000) &&
        !(a->previousTerrainFlags & 0xF000) && below <= 1 && above == 1) {
        v = ((a->forwardSpeed >> 10) + (a->forwardBias >> 8)) >> 1;
        if (v < 4)
            v = 4;
        if (v > 0x12)
            v = 0x12;
        *vert -= v << 9;
        a->flags40 &= ~1;
    }

    ScenePool_UpdateActorDrawablePosition(a);
    Modifier_InitDrainState((u8 *)a + 0xB0, a->model + 0x20, 0x78, 0);
    ActorStats_ConfigureScaledParameters(a, 0x1000, 0x2000, 0xFFFFE000, 0x2000, 0, 0x4000);
    CharacterModel_SetPrimaryAppearance(a->model, &a->position);
    ActorMarkerPool_HideMarker(a->marker);
    SceneActorStyle_Initialize(a->styleBytes, 0, 0, 0, 0, 0, 0, 0x64, 0x64);
    ActorAction_InitializeMode(a, a, 0);
    CharacterPreview_InitializePalette(&a->unk68, a->model, 1);
    a->index = -1;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0803CEE8);
#endif
