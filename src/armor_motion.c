#include "global.h"
#include "engine.h"
#include "ram.h"
#include "contact_actor.h"

struct ArmorActor {
    u8 pad0[0x94];
    u8 animationData[0x20];
    u8 animation[8];
    u32 renderFlags;
    u8 padC0[4];
    u16 animationSpeed;
    u8 padC6[0xA6C];
    u16 flags;
    u8 padB34[4];
    u8 variant;
};

struct ArmorAnimationRequest {
    u16 index;
    u8 flag;
    u8 mode;
};

extern const u8 gUnk_0824EB60[];

extern const u16 gUnk_0824E920[][2][36];


void ActorController_SetAnimation(struct ArmorActor *s, struct ArmorAnimationRequest *request,
                                  u32 flags)
{
    u32 index = request->index;
    u32 side;
    s16 variant;
    s32 kind;
    s8 mode;
    {
        u32 facing = s->flags & 1;
        side = 1;
        if (facing)
            side = 0;
    }
    kind = s->variant;
    if (s->flags & 0x200)
        kind += 4;
    variant = gUnk_0824EB60[kind];
    switch ((s32)request->mode) {
        case 0:
        default:
            mode = 2;
            break;
        case 1:
            mode = 1;
            break;
        case 2:
            mode = 0;
            break;
    }
    if (request->flag)
        s->renderFlags |= 4;
    else
        s->renderFlags &= ~4;
    Anim_SetKeyframes(s->animation, s->animationData, gUnk_0824E920[variant][side][index], mode,
                      (u16)flags);
    s->animationSpeed = 0x40;
    s->flags &= ~0x20;
}


void Armor_SetAnimationFromStart(u32 a, u32 b)
{
    ActorController_SetAnimation(a, b, 0);
}

#define gArmorScatterOffset gUnk_0824EB74

struct ArmorEffectActor {
    u8 pad0[0x48];
    u8 position[12];
};

extern const s32 gUnk_0824EB74[];


void Armor_SpawnScatterEffect(struct ArmorEffectActor *s)
{
    u32 lifetime;
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    lifetime = *(const u16 *)((gRandomIndex << 1) + (u32)gRandomTable);
    lifetime &= 0x3f;
    lifetime += 0x14;
    SpriteEffectController_Create(s->position, gUnk_0824EB74, 0x1c1a, 0xd1b8, 3, 0, 0x31, 0, 1, 0,
                                  0, lifetime, 4, 1, 0x200004, 30, 8, 8);
}

struct SceneEffectActor {
    u8 unknown0[0x48];
    struct ContactPosition position;
};

struct SpriteEffectController;

extern const struct ContactPosition gUnk_0824EB7C;


void SceneActor_SpawnSpriteEffect(struct SceneEffectActor *p)
{
    SpriteEffectController_Create(&p->position, &gUnk_0824EB7C, 0x1c1a, 0xd1b8, 0x12, 0, 0x11, 0, 1,
                                  0, 0, 0, 4, 1, 0x400008, 30, 8, 8);
}

#define gArmorFacingAnimations gUnk_0824EB84

struct ArmorCollider {
    u16 unknown0[3];
    u16 flags;
};

struct ArmorCollisionGate {
    u32 unknown0;
    u32 flags;
};

/* Observed component offsets; collision interpretations are tentative. */

struct ArmorFacingActor {
    u8 unknown0[0x180];
    struct ArmorCollisionGate collisionGate;
    u8 unknown188[8];
    struct ArmorCollider collider190;
    u8 unknown198[0x4c];
    struct ArmorCollider collider1E4;
    u8 unknown1EC[0x4c];
    struct ArmorCollider collider238;
    u8 unknown240[0x4c];
    struct ArmorCollider collider28C;
    u8 unknown294[0x89e];
    u16 flags;
    u8 unknownB34[5];
    u8 facing;
    u8 unknownB3A[6];
    u8 state;
    u8 phase;
};

extern const u32 gUnk_0824EB84[];

void Armor_SetAnimationFromStart(u32, u32);

void Armor_EnterFacingAnimation(struct ArmorFacingActor *actor)
{
    u8 *p = (u8 *)actor;
    u8 *state = p + 0xb40;
    if (*state == 0) {
        u32 four = 4;
        Armor_SetAnimationFromStart((u32)p, (u32)&gUnk_0824EB84[((p[0xb39] + 0x60) >> 6) & 3]);
        *state = 0xff;
        p[0xb41] = 1;
        ((struct ArmorCollider *)(p + 0x238))->flags &= ~four;
        ((struct ArmorCollider *)(p + 0x1e4))->flags &= ~four;
        ((struct ArmorCollider *)(p + 0x28c))->flags &= ~four;
        ((struct ArmorCollider *)(p + 0x190))->flags |= four;
        {
            u32 one = 1;
            ((struct ArmorCollisionGate *)(p + 0x180))->flags &= ~one;
        }
        *(u16 *)(p + 0xb32) &= ~0x20;
    }
}

#define gArmorWalkAnimations gUnk_0824EB94

#define gSineTable           gUnk_086149C4

// Tentative Armor family: on the first frame start the walk animation for the

// current facing quadrant, then every frame drive the motion vector from the

// facing angle and the given speed. Field names past 0x50 are provisional.

struct ArmorSpriteState {
    u16 unknown0, unknown2, unknown4, flags;
};

struct ArmorControlState {
    u32 unknown0, flags;
};

struct ArmorMotionVector {
    s16 x, y, z;
};

struct ArmorWalkController {
    u8 pad000[0x48];
    u8 axisX; // 0x48, 0x80-centered
    u8 pad049[0x4C - 0x49];
    u8 axisZ; // 0x4C, 0x80-centered
    u8 pad04D[0x50 - 0x4D];
    s16 motionX; // 0x50
    s16 motionY; // 0x52
    s16 motionZ; // 0x54
    u8 pad056[0xC0 - 0x56];
    u16 stepSoundLock; // 0xC0, nonzero suppresses the surface sound
    u8 pad0C2[0xC4 - 0xC2];
    u16 animationDuration; // 0xC4, turnDuration in Armor_UpdatePositiveTurn
    u8 pad0C6[0xC8 - 0xC6];
    u16 surfaceType; // 0xC8, 1 and 5 quarter the speed, 2 and 5 play a sound
    u8 pad0CA[0xB32 - 0xCA];
    u16 flagsB32; // 0xB32
    u8 padB34[5];
    u8 facing; // 0xB39
    u8 padB3A[6];
    u8 walkState;    // 0xB40
    u8 walkComplete; // 0xB41
};

extern const u32 gArmorWalkAnimations[];


void Sound_PlaySongChecked(u32);

static inline void ClearControlFlags(struct ArmorControlState *c, s32 mask)
{
    c->flags &= mask;
}

static inline s32 RoundMotion(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -((-x) >> 12);
}

static inline void SetMotion(struct ArmorMotionVector *v, s32 angle, s32 speed)
{
    v->x = RoundMotion(gSineTable[(angle + 64) & 255] * speed);
    v->y = 0;
    v->z = RoundMotion(gSineTable[angle] * speed);
}

void Armor_UpdateWalk(struct ArmorWalkController *p, s32 speed, s32 duration)
{
    u8 *s = &p->walkState;
    if (*s == 0) {
        u32 four = 4;
        Armor_SetAnimationFromStart((u32)p,
                                    (u32)&gArmorWalkAnimations[((p->facing + 0x60) >> 6) & 3]);
        (*s)++;
        p->walkComplete = 1;
        p->animationDuration = duration;
        ((struct ArmorSpriteState *)((u8 *)p + 0x238))->flags &= ~four;
        ((struct ArmorSpriteState *)((u8 *)p + 0x1E4))->flags &= ~four;
        ((struct ArmorSpriteState *)((u8 *)p + 0x28C))->flags &= ~four;
        ((struct ArmorSpriteState *)((u8 *)p + 0x190))->flags |= four;
        ClearControlFlags((struct ArmorControlState *)((u8 *)p + 0x180), ~1);
        p->flagsB32 &= ~0x20;
    }
    SetMotion((struct ArmorMotionVector *)&p->motionX, p->facing, speed);
    if (p->motionX == 0) {
        s32 d = 0x80 - p->axisX;
        if (d > 0x10)
            p->motionX = 3;
        else if (d < -0x10)
            p->motionX = -3;
    } else if (p->motionZ == 0) {
        s32 d = 0x80 - p->axisZ;
        if (d > 0x10)
            p->motionZ = 3;
        else if (d < -0x10)
            p->motionZ = -3;
    }
    if ((p->surfaceType & 3) == 1) {
        p->motionX >>= 2;
        p->motionZ >>= 2;
    }
    switch (p->surfaceType) {
        case 2:
            if (p->stepSoundLock == 0)
                Sound_PlaySongChecked(0xC2);
            break;
        case 5:
            if (p->stepSoundLock == 0)
                Sound_PlaySongChecked(0xC3);
            break;
    }
}


void Armor_UpdateWalkNormal(u32 a)
{
    Armor_UpdateWalk(a, 0xA, 0x40);
}

void Camera_RequestShake(s32);

void Armor_UpdateWalkHeavy(u32 p)
{
    Armor_UpdateWalk(p, 7, 0x5b);
    Camera_RequestShake(3);
}

void Armor_UpdateWalkFast(u32 a)
{
    Armor_UpdateWalk(a, 0xF, 0x2A);
}

void Armor_UpdateWalkFaster(u32 a)
{
    Armor_UpdateWalk(a, 0x12, 0x23);
}



void Armor_UpdateWalkNormalWithScatterEffect(u8 *p)
{
    Armor_UpdateWalk(p, 10, 0x40);
    if ((p[0xB42] & 0x1f) == 0)
        Armor_SpawnScatterEffect(p);
}


void Armor_UpdateWalkFastWithTrailEffect(u8 *p)
{
    Armor_UpdateWalk(p, 15, 0x2A);
    if ((p[0xB42] & 0xF) == 0)
        SceneActor_SpawnSpriteEffect(p);
}

/* tentative: directional run motion along facing with edge nudge */

/* tentative: picks a directional animation and moves along the facing with a nudge near the cell edge */

struct ActorAnimationFlags {
    u16 unk0, unk2, unk4;
    u32 flags : 16;
};

struct ActorStateFlags {
    u32 flags : 16;
};

struct ActorMotionFlags {
    u32 unk0, flags;
};

struct ActorMotionVector {
    s16 x, y, z;
};

extern u32 gUnk_0824EBA4[];


void Armor_UpdateRunMotion(u8 *p, s32 speed, u32 timer)
{
    u8 *s = p + 0xB40;
    s16 zero;
    struct ActorMotionVector *v;

    if (*s == 0) {
        u32 four = 4;
        Armor_SetAnimationFromStart((u32)p, (u32)&gUnk_0824EBA4[((p[0xB39] + 0x60) >> 6) & 3]);
        (*s)++;
        p[0xB41] = 1;
        *(u16 *)(p + 0xC4) = timer;
        ((struct ActorAnimationFlags *)(p + 0x238))->flags &= ~four;
        ((struct ActorAnimationFlags *)(p + 0x1E4))->flags &= ~four;
        ((struct ActorAnimationFlags *)(p + 0x28C))->flags &= ~four;
        ((struct ActorAnimationFlags *)(p + 0x190))->flags =
            four | ((struct ActorAnimationFlags *)(p + 0x190))->flags;
        {
            struct ActorMotionFlags *m = (void *)(p + 0x180);
            u32 one = ~1;
            m->flags &= one;
        }
        ((struct ActorStateFlags *)(p + 0xB32))->flags |= 0x20;
    }
    {
        u8 *ap = p + 0xB39;
        struct ActorMotionVector *w = v = (void *)(p + 0x50);
        u8 a = *ap;
        s32 t;
        w->x = RoundMotion(gUnk_086149C4[(a + 64) & 255] * speed);
        w->y = 0;
        t = RoundMotion(gUnk_086149C4[a] * speed);
        zero = 0;
        w->z = t;
    }
    if (*(s16 *)(p + 0x50) == 0) {
        s32 d = 0x80 - p[0x48];
        if (d > 16)
            *(s16 *)(p + 0x50) = 3;
        else if (d < -16) {
            u32 n = 0xFFFD;
            *(u16 *)(p + 0x50) = n;
        }
    } else if (*(s16 *)(p + 0x54) == 0) {
        s32 d = 0x80 - p[0x4C];
        if (d > 16)
            *(s16 *)(p + 0x54) = 3;
        else if (d < -16) {
            u32 n = 0xFFFD;
            *(u16 *)(p + 0x54) = n;
        }
    }
    switch (*(u16 *)(p + 0xC8)) {
        case 2:
            if (*(u16 *)(p + 0xC0) == zero)
                Sound_PlaySongChecked(0xC2);
            break;
        case 5:
            if (*(u16 *)(p + 0xC0) == zero)
                Sound_PlaySongChecked(0xC3);
            break;
    }
}


void Armor_UpdateRun(u32 a)
{
    Armor_UpdateRunMotion(a, 0x14, 0x20);
}


void Armor_UpdateRunWithScatterEffect(u8 *p)
{
    Armor_UpdateRunMotion(p, 20, 0x20);
    if ((p[0xB42] & 0x7) == 0)
        Armor_SpawnScatterEffect(p);
}

void Armor_UpdateRunWithTrailEffect(u8 *p)
{
    Armor_UpdateRunMotion(p, 20, 0x20);
    if ((p[0xB42] & 0x7) == 0)
        SceneActor_SpawnSpriteEffect(p);
}

/* tentative: strafe motion along 0xB3B with backward animation mode */

/* tentative: picks a directional animation and moves along the facing with a nudge near the cell edge */

extern u32 gUnk_0824EBB4[];

void Armor_UpdateStrafeMotion(u8 *p, s32 speed, u32 timer)
{
    u8 *s = p + 0xB40;
    struct ActorMotionVector *v;

    if (*s == 0) {
        u32 four = 4;
        u8 a = p[0xB39];
        u32 dir = ((a + 0x60) >> 6) & 3;
        u32 mode;
        s32 d = p[0xB3B] - a;
        mode = 0;
        if ((u32)((d & 0xFF) - 0x41) <= 0x7E)
            mode = 2;
        ActorController_SetAnimation((u32)p, (u32)&gUnk_0824EBB4[dir], mode);
        (*s)++;
        p[0xB41] = 1;
        *(u16 *)(p + 0xC4) = timer;
        ((struct ActorAnimationFlags *)(p + 0x238))->flags &= ~four;
        ((struct ActorAnimationFlags *)(p + 0x1E4))->flags &= ~four;
        ((struct ActorAnimationFlags *)(p + 0x28C))->flags &= ~four;
        ((struct ActorAnimationFlags *)(p + 0x190))->flags =
            four | ((struct ActorAnimationFlags *)(p + 0x190))->flags;
        {
            struct ActorMotionFlags *m = (void *)(p + 0x180);
            u32 one = ~1;
            m->flags &= one;
        }
        ((struct ActorStateFlags *)(p + 0xB32))->flags |= 0x20;
    }
    {
        u8 *ap = p + 0xB3B;
        struct ActorMotionVector *w = v = (void *)(p + 0x50);
        u8 a = *ap;
        s32 t;
        w->x = RoundMotion(gUnk_086149C4[(a + 64) & 255] * speed);
        w->y = 0;
        t = RoundMotion(gUnk_086149C4[a] * speed);
        w->z = t;
    }
    if ((*(u16 *)(p + 0xC8) & 3) == 1) {
        *(s16 *)(p + 0x50) >>= 2;
        *(s16 *)(p + 0x54) >>= 2;
    }
    switch (*(u16 *)(p + 0xC8)) {
        case 2:
            if (*(u16 *)(p + 0xC0) == 0)
                Sound_PlaySongChecked(0xC2);
            break;
        case 5:
            if (*(u16 *)(p + 0xC0) == 0)
                Sound_PlaySongChecked(0xC3);
            break;
    }
}


void Armor_UpdateStrafeNormal(u32 a)
{
    Armor_UpdateStrafeMotion(a, 0x7, 0x40);
}

void Armor_UpdateStrafeFast(u32 a)
{
    Armor_UpdateStrafeMotion(a, 0xA, 0x2A);
}

void Armor_UpdateStrafeFaster(u32 a)
{
    Armor_UpdateStrafeMotion(a, 0xD, 0x23);
}


void Armor_UpdateStrafeNormalWithScatterEffect(u8 *p)
{
    Armor_UpdateStrafeMotion(p, 7, 0x40);
    if ((p[0xB42] & 0x1f) == 0)
        Armor_SpawnScatterEffect(p);
}

#define gArmorPositiveTurnAnimations gUnk_0824EBC4

// Tentative Armor family: turn by positive quarter turns until facing matches target.

// Explicit subobject views retain the original base-plus-field addressing.

struct ArmorTurnController {
    u8 pad000[0xC4];
    u16 turnDuration;
    u8 pad0C6[0x180 - 0xC6];
    struct ArmorControlState control;
    u8 pad188[8];
    struct ArmorSpriteState sprite190;
    u8 pad198[0x1E4 - 0x198];
    struct ArmorSpriteState sprite1E4;
    u8 pad1EC[0x238 - 0x1EC];
    struct ArmorSpriteState sprite238;
    u8 pad240[0x28C - 0x240];
    struct ArmorSpriteState sprite28C;
    u8 pad294[0xB32 - 0x294];
    u16 flagsB32;
    u8 padB34[5];
    u8 facing;
    u8 targetFacing;
    u8 padB3B[5];
    u8 turnState;
    u8 turnComplete;
    u8 padB42;
    u8 animationComplete;
};

extern const u32 gArmorPositiveTurnAnimations[];

void Armor_UpdatePositiveTurn(struct ArmorTurnController *p, s32 duration)
{
    u8 *s = &p->turnState;
    u8 state = *s;
    switch (state) {
        case 0: {
            u32 four = 4;
            if (p->facing == p->targetFacing) {
                *s = 0xFF;
                p->turnComplete = 1;
                break;
            }
            Armor_SetAnimationFromStart(
                (u32)p, (u32)&gArmorPositiveTurnAnimations[((p->facing + 0x60) >> 6) & 3]);
            (*s)++;
            p->turnComplete = state;
            p->turnDuration = duration;
            ((struct ArmorSpriteState *)((u8 *)p + 0x238))->flags &= ~four;
            ((struct ArmorSpriteState *)((u8 *)p + 0x1E4))->flags &= ~four;
            ((struct ArmorSpriteState *)((u8 *)p + 0x28C))->flags &= ~four;
            ((struct ArmorSpriteState *)((u8 *)p + 0x190))->flags |= four;
            {
                u32 one = 1;
                ((struct ArmorControlState *)((u8 *)p + 0x180))->flags &= ~one;
            }
            p->flagsB32 &= ~0x20;
            break;
        }
        case 1:
            if (p->animationComplete != 0) {
                p->facing += 0x40;
                if (p->facing != p->targetFacing) {
                    *s = 0;
                } else {
                    *s = 0xFF;
                    p->turnComplete = state;
                }
            }
            break;
    }
}

#define gArmorNegativeTurnAnimations gUnk_0824EBD4

// Tentative Armor family: turn by negative quarter turns until facing matches target.

// Explicit subobject views retain the original base-plus-field addressing.

extern const u32 gArmorNegativeTurnAnimations[];

void Armor_UpdateNegativeTurn(struct ArmorTurnController *p, s32 duration)
{
    u8 *s = &p->turnState;
    u8 state = *s;
    switch (state) {
        case 0: {
            u32 four = 4;
            if (p->facing == p->targetFacing) {
                *s = 0xFF;
                p->turnComplete = 1;
                break;
            }
            Armor_SetAnimationFromStart(
                (u32)p, (u32)&gArmorNegativeTurnAnimations[((p->facing + 0x60) >> 6) & 3]);
            (*s)++;
            p->turnComplete = state;
            p->turnDuration = duration;
            ((struct ArmorSpriteState *)((u8 *)p + 0x238))->flags &= ~four;
            ((struct ArmorSpriteState *)((u8 *)p + 0x1E4))->flags &= ~four;
            ((struct ArmorSpriteState *)((u8 *)p + 0x28C))->flags &= ~four;
            ((struct ArmorSpriteState *)((u8 *)p + 0x190))->flags |= four;
            {
                u32 one = 1;
                ((struct ArmorControlState *)((u8 *)p + 0x180))->flags &= ~one;
            }
            p->flagsB32 &= ~0x20;
            break;
        }
        case 1:
            if (p->animationComplete != 0) {
                p->facing -= 0x40;
                if (p->facing != p->targetFacing) {
                    *s = 0;
                } else {
                    *s = 0xFF;
                    p->turnComplete = state;
                }
            }
            break;
    }
}


void Armor_UpdatePositiveTurnNormal(u8 *p)
{
    Armor_UpdatePositiveTurn(p, 64);
}


void Armor_UpdateNegativeTurnNormal(u8 *p)
{
    Armor_UpdateNegativeTurn(p, 64);
}


void Armor_UpdatePositiveTurnFast(s32 a)
{
    Armor_UpdatePositiveTurn(a, 0x2a);
}


void Armor_UpdateNegativeTurnFast(s32 a)
{
    Armor_UpdateNegativeTurn(a, 0x2a);
}

void Armor_UpdatePositiveTurnFaster(s32 a)
{
    Armor_UpdatePositiveTurn(a, 0x23);
}

void Armor_UpdateNegativeTurnFaster(s32 a)
{
    Armor_UpdateNegativeTurn(a, 0x23);
}

void Armor_UpdatePositiveTurnSlow(s32 a)
{
    Armor_UpdatePositiveTurn(a, 0x5b);
}
