#include "global.h"
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

#define ACTOR_OFFSET(member) ((u32) & ((struct WirelessSceneActor *)0)->member)
#define FIELD(p, off, T)     (*(T *)((u8 *)(p) + (off)))

struct Vec16 {
    s32 a, b, c, d;
};
struct ShortVector {
    s16 x, y, z;
};
struct ByteValue {
    u8 value;
};

static inline u8 TestBit(u8 *value, u32 input)
{
    u32 mask = input;
    if (value[0x5EE] & mask)
        return 1;
    return 0;
}

static inline void ClearBits(u8 *p, const s32 input)
{
    u8 *value = &((struct WirelessSceneActor *)p)->stateFlags;
    s32 mask = input;
    ((struct ByteValue *)value)->value &= mask;
}

static inline u32 TestFlags(u32 *p, u32 input)
{
    u32 mask = input;
    return *p & mask;
}

static inline void ModifyFlags(u32 *p, u32 value, u32 set)
{
    u32 mask = value;
    if (set)
        *p |= mask;
    else
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
    struct WirelessSceneActor *actor = (struct WirelessSceneActor *)slot;
    struct WirelessSceneActor *view = (struct WirelessSceneActor *)actor;
    s32 speed;
    s32 *speedB, *speedA, *vert;
    s32 *pprevZ;
    u32 *flags;
    s32 dx, dz, frames, lim, h, heading;
    u32 above, below;
    s32 *pprevX, *ox, *i1, *i2;
    u16 *ph;
    struct ShortVector pos;
    u32 one;
    u32 fieldOffset;
    s8 *earlyFrames;
    s32 *earlyForward;

    {
        u32 *p = &actor->actionFlags;
        u32 mask;
        *p = 0;
        actor->timer858 = 0;
        mask = 0x20;
        if (!(*p & mask) && (FIELD(actor, 0xC, u32) & 0x800)) {
            u32 bit = 4;
            bit |= actor->stateFlags;
            actor->stateFlags = bit;
        }
    }
    if (TestBit((u8 *)view, 4)) {
        ClearBits(slot, ~6);
        RestartState(item, 11, 0);
    } else {
        if (TestBit(slot, 2)) {
            ClearBits(slot, ~2);
            RestartState(item, 10, 0);
        }
    }
    if (view->flags40 & 2)
        view->lateralSpeed = 0;
    {
        u32 controls = view->flags40;
        one = 1;
        if (controls & one)
            view->verticalSpeed = 0;
    }
    {
        u32 anchorIndex = FIELD(item, 0x40, u32) * 16;
        u8 *xbase = pool + 0x2324;
        u8 *zbase;
        FIELD(item, 0x58, s32) = *(s32 *)(xbase + anchorIndex) - FIELD(slot, 0x10, s32);
        zbase = pool + 0x232C;
        FIELD(item, 0x5C, s32) = *(s32 *)(zbase + anchorIndex) - FIELD(slot, 0x18, s32);
    }
    if (FIELD(item, 0x68, u32) <= 11)
        gUnk_0860585C[FIELD(item, 0x68, u32)](pool, slot, item, index);
    fieldOffset = ACTOR_OFFSET(impulseFrames);
    earlyFrames = (s8 *)((u8 *)actor + fieldOffset);
    if (*earlyFrames <= 0)
        ScenePool_ApplyMotionControls(slot, item);
    Callback_InvokeActor(actor, FIELD(actor, 0xB0, void *));
    fieldOffset = ACTOR_OFFSET(forwardSpeed);
    earlyForward = (s32 *)((u8 *)actor + fieldOffset);
    *earlyForward -= 0x20;
    {
        u32 mask = 0x20;
        u32 *p = &((struct WirelessSceneActor *)slot)->actionFlags;
        u32 value = *p & mask;
        flags = p;
        if (!value)
            ScenePool_ClampMotionSpeeds(actor);
    }
    actor->lateralSpeed = Div(actor->lateralSpeed * 90, 100);
    if (TestFlags(flags, one)) {
        ((struct WirelessSceneActor *)slot)->forwardSpeed = 0;
        ((struct WirelessSceneActor *)slot)->lateralSpeed = 0;
        ((struct WirelessSceneActor *)slot)->forwardBias = 0;
        FIELD(slot, 0xC8, s32) = 0;
        *earlyFrames = 0;
    }
    fieldOffset = 0;
    speed = actor->forwardSpeed + actor->forwardBias;
    {
        s32 limit = 0xF000;
        speedA = &actor->forwardSpeed;
        speedB = &actor->forwardBias;
        if (speed > limit)
            speed = 0xF000;
    }
    actor->verticalSpeed += 0x200;
    frames = actor->impulseFrames;
    vert = &actor->verticalSpeed;
    if (frames > 0) {
        actor->offsetX += actor->impulse[0];
        actor->impulse[0] += Div(-actor->impulse[0], actor->impulseFrames);
        actor->impulse[1] += Div(-actor->impulse[1], actor->impulseFrames);
        actor->impulse[2] += Div(-actor->impulse[2], actor->impulseFrames);
        actor->impulseFrames--;
        ox = &actor->offsetX;
        i1 = &actor->impulse[1];
        i2 = &actor->impulse[2];
    } else {
        Memory_Clear(actor->impulse, 0x10);
        ox = &actor->offsetX;
        i1 = &actor->impulse[1];
        i2 = &actor->impulse[2];
    }
    Actor_ClampMovementSpeed(actor);
    actor->motion[0] = speed + *ox - actor->previousX;
    actor->motion[1] = *vert + *i1;
    actor->motion[2] = actor->lateralSpeed + *i2;
    dx = (*ox - actor->previousX) >> 12;
    dz = (actor->position.z - actor->previousZ) >> 12;
    lim = 0x3000;
    pprevX = &actor->previousX;
    pprevZ = &actor->previousZ;
    if (speed > lim)
        dx = 0x80;
    if (dx == 0 && dz == 0) {
        actor->heading = 0xFFFF;
        ph = &actor->heading;
    } else {
        actor->heading = (Math_Atan2(dx, dz) >> 5) << 5;
        ph = &actor->heading;
    }
    *pprevX = *ox;
    *pprevZ = actor->position.z;
    Terrain_ResolveActorMotion(actor);
    above = (u32) ~(actor->heightB >> 12) >> 31;
    below = (u32) ~(actor->heightA >> 12) >> 31;
    if (actor->terrainFlags != actor->previousTerrainFlags && (actor->terrainFlags & 0x1000) &&
        !(actor->previousTerrainFlags & 0xF000) && below <= 1 && above == 1) {
        s32 v = ((*speedA >> 10) + (*speedB >> 8)) >> 1;
        if (v < 4)
            v = 4;
        if (v > 0x12)
            v = 0x12;
        *vert -= v << 9;
        actor->flags40 &= ~1;
    }
    {
        Character_GetHeightAdjustedPosition(actor, (s16 *)&pos);
        SpatialIndex_SetPosition((u8 *)actor + 0x56C, pos.x, pos.y, pos.z);
        h = *ph;
        heading = (s16)h;
        if (heading < 0) {
            SpatialIndex_SetPosition((u8 *)actor + 0x52C, pos.x, pos.y, pos.z);
        } else {
            struct ShortVector off;
            struct ShortVector *o = &off;
            s32 angle = (s16)*ph;
            s32 radius = 20;
            o->x = Scale(gUnk_086149C4[(angle + 0x40) & 0xFF] * radius);
            o->y = 0;
            o->z = Scale(gUnk_086149C4[angle & 0xFF] * radius);
            SpatialIndex_SetPosition((u8 *)actor + 0x52C, pos.x + off.x, pos.y, pos.z + off.z);
        }
    }
    ScenePool_UpdateProximityCollider(slot);
    ScenePool_UpdateActorDrawablePosition(slot);
    CharacterModel_UpdateAccessoryAnimations(slot + 0x600);
    if (item[0x25] == 3) {
        u16 *timer = &FIELD(actor, 0x308, u16);
        u32 current = *timer;
        u16 t = current;
        if (t) {
            if (t == 1) {
                Anim_SetKeyframes(slot + 0x660, slot + 0x7E0, 9, 2, 0);
                current = *timer;
            }
            *timer = current - 1;
            Anim_Step(slot + 0x660, slot + 0x7E0);
        }
    }
    ActorAction_TickRecovery(actor, CharacterModel_GetPrimaryPaletteIndex(slot + 0x600));
    if ((TestFlags(flags, 0x10)) && item[0x25] == 3) {
        if (item[0x3B])
            goto countdown;
        if (ScenePool_UpdateProximityCooldown(slot)) {
            ModifierAction_SpawnBurst(actor, CharacterModel_GetPrimaryPaletteIndex(slot + 0x600));
            ((struct WirelessSceneActor *)slot)->timer85C = 480;
            item[0x3B] = 0xF0;
        }
    }
    if (item[0x3B]) {
        u32 enable;
    countdown:
        ((struct ByteValue *)(item + 0x3B))->value--;
        ((struct ByteValue *)((u8 *)actor + 0x57A))->value = 0;
        FIELD(actor, 0x68, u32) = 0;
        FIELD(actor, 0x326, u16) = 0;
        {
            enable = 1;
            ClearBits((u8 *)actor, ~1);
        }
        if (item[0x3B] == 0)
            PalFade_Start(actor, CharacterModel_GetPrimaryPaletteIndex(slot + 0x600),
                          FIELD(actor, 0x7E, u16), 0, 0, 0x10, enable, enable);
    }
    if (!(TestFlags(flags, 0x21))) {
        if (((u8 *)actor)[0x57A])
            FIELD(item, 0x60, u32) += FIELD(actor, 0x578, u16);
        ActorStats_ApplyPendingDamage(actor);
        ActorParticles_UpdateEmitter(actor, (u8 *)actor + 0x44, FIELD(actor, 0x5C, u32));
    }
    PalFade_Update(actor);
    if (TestFlags(flags, 2)) {
        CharacterModel_SetAnimationFlags(slot + 0x600, 1);
        ActorMarkerPool_HideMarker((s8)slot[0x850]);
    } else {
        CharacterModel_ClearAnimationFlags(slot + 0x600, 1);
        ActorMarkerPool_ShowMarker((s8)slot[0x850]);
    }
    {
        u32 *visible;
        if (!(TestFlags(flags, 8))) {
            ModifyFlags(&((struct WirelessSceneActor *)slot)->motionFlags, ~1, 0);
            visible = &((struct WirelessSceneActor *)slot)->motionFlags;
        } else {
            ModifyFlags(&((struct WirelessSceneActor *)slot)->motionFlags, 1, 1);
            visible = &((struct WirelessSceneActor *)slot)->motionFlags;
        }
        if (!(TestFlags(flags, 4))) {
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
    Character_UpdateTimedEffect((u8 *)actor + 0x68);
    if (!(TestFlags(flags, 0x20)) && (actor->flags40 & 4))
        ActorParticles_SpawnImpactBurst(actor);
    FIELD(item, 0x14, struct Vec16) = FIELD(actor, 0x10, struct Vec16);
}
