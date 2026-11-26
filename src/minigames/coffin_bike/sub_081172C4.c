#include "global.h"
#include "system/game_state/game_state.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "scene/area_effect/area_effect_link.h"
#include "minigames/coffin_bike/glove.h"

s32 Div(s32, s32);
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
    return &self->previousPosition.z;
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
        self->outX = (h + (*Motion_GetForwardOffset(self))) - self->previousPosition.x;
        self->outY = (*Motion_GetVerticalOffset(self)) + (*Motion_GetRecoilY(self));
        self->outZ = (*Motion_GetLateralOffset(self)) + (*Motion_GetRecoilZ(self));
        dx = ((*Motion_GetForwardOffset(self)) - self->previousPosition.x) >> 12;
        dz = (self->position.z - *Motion_GetPreviousZ(self)) >> 12;
        lim = 0x3000;
        pprevX = &self->previousPosition.x;
        if (h > lim)
            dx = 0x80;
        if ((dx == 0) && (dz == 0))
            self->angle = 0xFFFF;
        else
            self->angle = (Math_Atan2(dx, dz) >> 5) << 5;
        *pprevX = (*Motion_GetForwardOffset(self));
        *Motion_GetPreviousZ(self) = self->position.z;
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
        Actor_UpdateShadowSpriteParts(self, &p->model);
        if (self->flags40 & 4)
            ActorParticles_SpawnImpactBurst(self);
        if (self->flags40 & 2)
            (*Motion_GetLateralOffset(self)) = 0;
        if (self->flags40 & 1)
            (*Motion_GetVerticalOffset(self)) = 0;
    }
    if (LinkParticipant_IsLocallySelected(p))
        Character_UpdateLoopingSound(self);
    CharacterModel_UpdateAccessoryAnimations(&p->model);
    p->timer++;
}
