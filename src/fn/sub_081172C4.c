#include "global.h"
#include "game_state.h"
#include "ram.h"
#include "engine_globals.h"
#include "area_effect_link.h"

union GloveAnimationKey {
    u32 word;
    u16 half[2];
};

struct GloveMotion {
    u8 pad0[4];
    u32 unk4;
    u32 unk8;
    u8 padC[0x18 - 0xC];
    s32 z;
    u8 pad1C[4];
    s32 outX, outY, outZ; /* 0x20 */
    u8 pad2C[0x38 - 0x2C];
    s32 unk38;
    s32 unk3C;
    u32 flags40;
    u8 pad44[0xC0 - 0x44];
    s32 unkC0;
    u8 padC4[0x524 - 0xC4];
    union GloveAnimationKey animation;
    u8 pad528[0x5AC - 0x528];
    s32 vy; /* 0x5AC */
    s32 y;  /* 0x5B0 */
    s32 vz; /* 0x5B4 */
    u8 pad5B8[4];
    s32 prevX; /* 0x5BC */
    u8 pad5C0[4];
    s32 prevZ; /* 0x5C4 */
    u8 pad5C8[4];
    s32 dx; /* 0x5CC */
    s32 dy; /* 0x5D0 */
    s32 dz; /* 0x5D4 */
    u8 pad5D8[4];
    s32 x; /* 0x5DC */
    u8 pad5E0[0x5EC - 0x5E0];
    s8 steps; /* 0x5EC */
    u8 pad5ED[3];
    u16 angle; /* 0x5F0 */
    u8 pad5F2[6];
};

struct Glove {
    u8 pad0[0x18];
    struct GloveMotion motion;
    u8 model[0x860 - 0x610];
    s8 marker, markerSlot;
    u8 pad862[2];
    u32 timer;
    u8 active;
    u8 pad869[3];
    void (*update)(void *);
    u8 pad870[0x8C4 - 0x870];
    u16 *input;
    u8 pad8C8[0x8E4 - 0x8C8];
    u32 previousInput;
    s32 height;
    u8 pad8EC[8];
    u16 flags, blinkTimer;
    s8 participant;
    u8 pad8F9[0x914 - 0x8F9];
    s32 launchSpeed;
    u8 launchPhase;
};
s32 Div(s32, s32);
void Memory_Clear(void *, u32);
s32 Math_Atan2(s32, s32);
void Terrain_ResolveActorMotion(struct GloveMotion *);
void Actor_UpdateShadowSpriteParts(struct GloveMotion *, void *);
void ActorParticles_SpawnImpactBurst(struct GloveMotion *);
void CharacterModel_UpdateAccessoryAnimations(void *);
void ScenePool_ClampMotionSpeeds(struct GloveMotion *);

void StageIconDisplay_Create(s32, s32, void *, void *);
void Glove_BeginStopping(void);
u8 LinkParticipant_IsLocallySelected(void *);
void Character_UpdateLoopingSound(void *);
static inline s32 *Motion_GetSpeed(struct GloveMotion *self)
{
    return &self->vy;
}
static inline s32 *Motion_GetTerrainSpeed(struct GloveMotion *self)
{
    return &self->unkC0;
}
static inline s32 *Motion_GetVerticalOffset(struct GloveMotion *self)
{
    return &self->y;
}
static inline s32 *Motion_GetForwardOffset(struct GloveMotion *self)
{
    return &self->x;
}
static inline s32 *Motion_GetRecoilY(struct GloveMotion *self)
{
    return &self->dy;
}
static inline s32 *Motion_GetRecoilZ(struct GloveMotion *self)
{
    return &self->dz;
}

static inline s32 *Motion_GetLateralOffset(struct GloveMotion *self)
{
    return &self->vz;
}
static inline s32 *Motion_GetPreviousZ(struct GloveMotion *self)
{
    return &self->prevZ;
}
// confidence=low: shows a stopping cue during launch ascent and slowdown.
#define Glove_UpdateRisingCue sub_081172C4
void Glove_UpdateRisingCue(struct Glove *p)
{
    struct GloveMotion *self = &p->motion;
    s32 h;
    s32 dx;
    s32 dz;
    u32 a;
    u32 b;
    if (p->active) {
        p->active = 0;
        p->launchPhase = 0;
        p->launchSpeed = 0;
        StageIconDisplay_Create(0, 0, Glove_BeginStopping, p);
        self->animation.half[1] = 0;
    }
    {
        u16 bit = 0x10;
        p->flags |= bit;
    }
    switch (p->launchPhase) {
        case 0:
            p->launchSpeed += 32;
            self->x += p->launchSpeed;
            self->animation.half[0] = 0x92;
            if (self->x > 0x40000)
                p->launchPhase = 3;
            break;

        case 3:
            if (p->launchSpeed > 0)
                p->launchSpeed -= 256;
            else
                p->launchPhase = 4;
            self->animation.half[0] = 0x88;
            self->x += p->launchSpeed;
            break;
    }

    ScenePool_ClampMotionSpeeds(self);
    h = (*Motion_GetSpeed(self)) + (*Motion_GetTerrainSpeed(self));
    (*Motion_GetVerticalOffset(self)) += 0x200;
    if (self->steps > 0) {
        (*Motion_GetForwardOffset(self)) += self->dx;
        self->dx += Div(-self->dx, self->steps);
        (*Motion_GetRecoilY(self)) += Div(-(*Motion_GetRecoilY(self)), self->steps);
        (*Motion_GetRecoilZ(self)) += Div(-(*Motion_GetRecoilZ(self)), self->steps);
        self->steps--;
    } else {
        Memory_Clear(&self->dx, 0x10);
    }
    {
        s32 *pprevX;
        s32 lim;
        self->outX = (h + (*Motion_GetForwardOffset(self))) - self->prevX;
        self->outY = (*Motion_GetVerticalOffset(self)) + (*Motion_GetRecoilY(self));
        self->outZ = (*Motion_GetLateralOffset(self)) + (*Motion_GetRecoilZ(self));
        dx = ((*Motion_GetForwardOffset(self)) - self->prevX) >> 12;
        dz = (self->z - *Motion_GetPreviousZ(self)) >> 12;
        lim = 0x3000;
        pprevX = &self->prevX;
        if (h > lim)
            dx = 0x80;
        if ((dx == 0) && (dz == 0))
            self->angle = 0xFFFF;
        else
            self->angle = (Math_Atan2(dx, dz) >> 5) << 5;
        *pprevX = (*Motion_GetForwardOffset(self));
        *Motion_GetPreviousZ(self) = self->z;
        Terrain_ResolveActorMotion(self);
        a = ((u32)(~(self->unk3C >> 12))) >> 31;
        b = ((u32)(~(self->unk38 >> 12))) >> 31;
        if (((((self->unk4 != self->unk8) && (self->unk4 & 0x1000)) && (!(self->unk8 & 0xF000))) &&
             (b <= 1)) &&
            (a == 1)) {
            s32 v =
                (((*Motion_GetSpeed(self)) >> 10) + ((*Motion_GetTerrainSpeed(self)) >> 8)) >> 1;
            if (v < 4)
                v = 4;
            if (v > 18)
                v = 18;
            (*Motion_GetVerticalOffset(self)) -= v << 9;
            self->flags40 &= ~1;
        }
        p->height = h;
        Actor_UpdateShadowSpriteParts(self, p->model);
        if (self->flags40 & 4)
            ActorParticles_SpawnImpactBurst(self);
        if (self->flags40 & 2)
            (*Motion_GetLateralOffset(self)) = 0;
        if (self->flags40 & 1)
            (*Motion_GetVerticalOffset(self)) = 0;
    }
    if (LinkParticipant_IsLocallySelected(p))
        Character_UpdateLoopingSound(self);
    CharacterModel_UpdateAccessoryAnimations(p->model);
    p->timer++;
}
