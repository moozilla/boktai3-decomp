#include "global.h"
#ifdef NONMATCHING
// fuzzy: 63.1%
// Open: motion-pointer lifetimes, initial control layout and register allocation differ.

struct WirelessSceneActor {
    u8 unknown0[4];
    u32 terrainFlags, previousTerrainFlags;
    u8 unknownC[4];
    struct {
        s32 x, y, z, w;
    } position;
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
    u8 unknown5ed;
    u8 stateFlags, unknown5ef;
    u16 heading;
    u8 unknown5f2[0xa];
    u32 actionFlags;
    u8 model[0x250];
    s8 marker;
    u8 unknown851[3];
    u32 index;
    u32 timer858, timer85C;
    u32 controlFlags;
};

typedef void (*ScenePoolStateFunc)(u8 *, u8 *, u8 *, u32);
extern ScenePoolStateFunc gUnk_0860585C[];
extern const s16 gUnk_086149C4[];

void ScenePool_ApplyMotionControls(u8 *, u8 *);
s32 Callback_InvokeActor(void *, void *);
void ScenePool_ClampMotionSpeeds(u8 *);
s32 Div(s32, s32);
void Actor_ClampMovementSpeed(u8 *);
s32 Math_Atan2(s32, s32);
void Terrain_ResolveActorMotion(u8 *);
void Character_GetHeightAdjustedPosition(void *, s16 *);
void SpatialIndex_SetPosition(void *, int, int, int);
void ScenePool_UpdateProximityCollider(u8 *);
void ScenePool_UpdateActorDrawablePosition(u8 *);
void CharacterModel_UpdateAccessoryAnimations(u8 *);
s32 Anim_Step(void *, void *);
s32 CharacterModel_GetPrimaryPaletteIndex(u8 *);
void ActorAction_TickRecovery(u8 *, s32);
s32 ScenePool_UpdateProximityCooldown(u8 *);
void ModifierAction_SpawnBurst(u8 *, s32);
void PalFade_Start(u8 *, s32, u16, u32, u32, u32, u32, u32);
void ActorStats_ApplyPendingDamage(u8 *);
void ActorParticles_UpdateEmitter(void *, void *, u32);
void PalFade_Update(u8 *);
void CharacterModel_SetAnimationFlags(void *, int);
void CharacterModel_ClearAnimationFlags(void *, int);
void ActorMarkerPool_HideMarker(int);
void ActorMarkerPool_ShowMarker(int);
u8 ScenePool_IsNearPrimaryPlayer(u8 *);
void SpatialIndex_InsertEntry(void *);
void ScenePool_ResetProximityContact(u8 *);
void Character_UpdateTimedEffect(void *);
void ActorParticles_SpawnImpactBurst(u8 *);

#define FIELD(p, off, T) (*(T *)((u8 *)(p) + (off)))

struct Vec16 {
    s32 a, b, c, d;
};

static inline u8 TestBit(u8 *value, u32 input)
{
    u32 mask = input;
    if (*value & mask)
        return 1;
    return 0;
}

static inline void ClearBits(u8 *p, s32 mask)
{
    *p &= mask;
}

static inline s32 Scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

static inline void RestartState(u8 *item, u32 state, u32 zero)
{
    FIELD(item, 0x68, u32) = state;
    FIELD(item, 0x44, u32) = zero;
    item[0x64] = 1;
    Memory_Clear(item + 0x6C, 0x100);
}

// confidence=low: per-frame update of one occupied scene-pool actor slot: state dispatch, motion integration,
// spatial-index placement, proximity/damage effects and marker visibility.
#define ScenePool_UpdateOccupiedActor sub_0803C114
void ScenePool_UpdateOccupiedActor(u8 *pool, u8 *slot, u8 *item, u32 index)
{
    struct WirelessSceneActor *a = (struct WirelessSceneActor *)slot;
    u8 *actor = slot;
    u8 *view = actor;
    s32 speed;
    u32 *flags;
    s32 *pos5DC, *pos5D0, *pos5D4;
    s32 *speedA, *speedB, *height;
    u16 *heading;
    s32 dx, dz;
    s16 position[4];
    s16 offset[4];
    u32 one;

    {
        u32 *p = &a->actionFlags;
        u32 mask;
        *p = 0;
        a->timer858 = 0;
        mask = 0x20;
        if (!(*p & mask) && (FIELD(actor, 0xC, u32) & 0x800)) {
            u32 bit = 4;
            actor[0x5EE] |= bit;
        }
    }
    if (TestBit(&view[0x5EE], 4)) {
        ClearBits(&slot[0x5EE], ~6);
        RestartState(item, 11, 0);
    } else {
        u8 *state = &slot[0x5EE];
        if (TestBit(state, 2)) {
            ClearBits(state, ~2);
            RestartState(item, 10, 0);
        }
    }
    if (a->flags40 & 2)
        a->lateralSpeed = 0;
    one = 1;
    if (a->flags40 & one)
        a->verticalSpeed = 0;
    {
        u32 anchorIndex = FIELD(item, 0x40, u32);
        FIELD(item, 0x58, s32) = ((struct Vec16 *)(pool + 0x2324))[anchorIndex].a - a->position.x;
        FIELD(item, 0x5C, s32) = ((struct Vec16 *)(pool + 0x232C))[anchorIndex].a - a->position.z;
    }
    if (FIELD(item, 0x68, u32) <= 11)
        gUnk_0860585C[FIELD(item, 0x68, u32)](pool, slot, item, index);
    if (a->impulseFrames <= 0)
        ScenePool_ApplyMotionControls(slot, item);
    Callback_InvokeActor(actor, FIELD(actor, 0xB0, void *));
    a->forwardSpeed -= 0x20;
    {
        u32 mask = 0x20;
        flags = &a->actionFlags;
        if (!(*flags & mask))
            ScenePool_ClampMotionSpeeds(actor);
    }
    a->lateralSpeed = Div(a->lateralSpeed * 90, 100);
    if (*flags & one) {
        a->forwardSpeed = 0;
        a->lateralSpeed = 0;
        a->forwardBias = 0;
        FIELD(slot, 0xC8, s32) = 0;
        a->impulseFrames = 0;
    }
    speedA = &a->forwardSpeed;
    speedB = &a->forwardBias;
    speed = *speedA + *speedB;
    if (speed > 0xF000)
        speed = 0xF000;
    height = &a->verticalSpeed;
    *height += 0x200;
    if (a->impulseFrames > 0) {
        s32 *drift;
        drift = &a->impulse[0];
        a->offsetX += *drift;
        pos5DC = &a->offsetX;
        *drift += Div(-*drift, a->impulseFrames);
        pos5D0 = &a->impulse[1];
        *pos5D0 += Div(-*pos5D0, a->impulseFrames);
        pos5D4 = &a->impulse[2];
        *pos5D4 += Div(-*pos5D4, a->impulseFrames);
        a->impulseFrames--;
    } else {
        Memory_Clear(actor + 0x5CC, 0x10);
        pos5DC = &a->offsetX;
        pos5D0 = &a->impulse[1];
        pos5D4 = &a->impulse[2];
    }
    Actor_ClampMovementSpeed(actor);
    a->motion[0] = speed + *pos5DC - a->previousX;
    a->motion[1] = *height + *pos5D0;
    a->motion[2] = a->lateralSpeed + *pos5D4;
    dx = (*pos5DC - a->previousX) >> 12;
    dz = (a->position.z - a->previousZ) >> 12;
    if (speed > 0x3000)
        dx = 0x80;
    if (dx == 0 && dz == 0) {
        heading = &a->heading;
        *heading = 0xFFFF;
    } else {
        s32 angle = (Math_Atan2(dx, dz) >> 5) << 5;
        heading = &a->heading;
        *heading = angle;
    }
    a->previousX = *pos5DC;
    a->previousZ = a->position.z;
    Terrain_ResolveActorMotion(actor);
    {
        u32 landedZ = (u32) ~(a->heightB >> 12) >> 31;
        u32 landedX = (u32) ~(a->heightA >> 12) >> 31;
        u32 cur = a->terrainFlags;
        u32 prev = a->previousTerrainFlags;
        if (cur != prev && (cur & 0x1000) && !(prev & 0xF000) && landedX <= 1 && landedZ == 1) {
            s32 n = ((*speedA >> 10) + (*speedB >> 8)) >> 1;
            if (n <= 3)
                n = 4;
            if (n > 0x12)
                n = 0x12;
            *height -= n << 9;
            a->flags40 &= ~1;
        }
    }
    Character_GetHeightAdjustedPosition(actor, position);
    SpatialIndex_SetPosition(actor + 0x56C, position[0], position[1], position[2]);
    if ((s16)*heading < 0) {
        SpatialIndex_SetPosition(actor + 0x52C, position[0], position[1], position[2]);
    } else {
        s32 angle = (s16)*heading;
        s32 radius = 20;
        offset[0] = Scale(gUnk_086149C4[(angle + 64) & 255] * radius);
        offset[1] = 0;
        offset[2] = Scale(gUnk_086149C4[angle & 255] * radius);
        SpatialIndex_SetPosition(actor + 0x52C, position[0] + offset[0], position[1],
                                 position[2] + offset[2]);
    }
    ScenePool_UpdateProximityCollider(slot);
    ScenePool_UpdateActorDrawablePosition(slot);
    CharacterModel_UpdateAccessoryAnimations(slot + 0x600);
    if (item[0x25] == 3) {
        u16 *timer = &FIELD(actor, 0x308, u16);
        u32 t = *timer;
        if (t) {
            if (t == 1) {
                Anim_SetKeyframes(slot + 0x660, slot + 0x7E0, 9, 2, 0);
                t = *timer;
            }
            *timer = t - 1;
            Anim_Step(slot + 0x660, slot + 0x7E0);
        }
    }
    ActorAction_TickRecovery(actor, CharacterModel_GetPrimaryPaletteIndex(slot + 0x600));
    if ((*flags & 0x10) && item[0x25] == 3) {
        if (item[0x3B])
            goto countdown;
        if (ScenePool_UpdateProximityCooldown(slot)) {
            ModifierAction_SpawnBurst(actor, CharacterModel_GetPrimaryPaletteIndex(slot + 0x600));
            a->timer85C = 480;
            item[0x3B] = 0xF0;
        }
    }
    if (item[0x3B]) {
    countdown:
        item[0x3B]--;
        actor[0x57A] = 0;
        FIELD(actor, 0x68, u32) = 0;
        FIELD(actor, 0x326, u16) = 0;
        actor[0x5EE] &= ~1;
        if (item[0x3B] == 0)
            PalFade_Start(actor, CharacterModel_GetPrimaryPaletteIndex(slot + 0x600),
                          FIELD(actor, 0x7E, u16), 0, 0, 0x10, 1, 1);
    }
    if (!(*flags & 0x21)) {
        if (actor[0x57A])
            FIELD(item, 0x60, u32) += FIELD(actor, 0x578, u16);
        ActorStats_ApplyPendingDamage(actor);
        ActorParticles_UpdateEmitter(actor, actor + 0x44, FIELD(actor, 0x5C, u32));
    }
    PalFade_Update(actor);
    if (*flags & 2) {
        CharacterModel_SetAnimationFlags(slot + 0x600, 1);
        ActorMarkerPool_HideMarker((s8)slot[0x850]);
    } else {
        CharacterModel_ClearAnimationFlags(slot + 0x600, 1);
        ActorMarkerPool_ShowMarker((s8)slot[0x850]);
    }
    {
        u32 *visible = &a->motionFlags;
        if (!(*flags & 8))
            *visible &= ~1;
        else
            *visible |= 1;
        if (!(*flags & 4)) {
            u16 *hold;
            if (ScenePool_IsNearPrimaryPlayer(slot))
                SpatialIndex_InsertEntry(slot + 0x52C);
            hold = &FIELD(slot, 0x59E, u16);
            if (*hold) {
                (*hold)--;
                *visible |= 1;
            } else {
                *visible &= ~1;
            }
        }
    }
    ScenePool_ResetProximityContact(slot);
    Character_UpdateTimedEffect(actor + 0x68);
    if (!(*flags & 0x20) && (a->flags40 & 4))
        ActorParticles_SpawnImpactBurst(actor);
    FIELD(item, 0x14, struct Vec16) = FIELD(actor, 0x10, struct Vec16);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0803C114);
#endif
