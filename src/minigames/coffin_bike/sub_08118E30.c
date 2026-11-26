#include "global.h"
#include "system/game_state/game_state.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "scene/areas/area_effect_link.h"
#include "minigames/coffin_bike/coffin_bike.h"

s32 Div(s32, s32);
s32 Math_Atan2(s32, s32);
void Terrain_ResolveActorMotion(struct GloveMotion *);
void PalFade_Update(struct GloveMotion *);
void Actor_UpdateShadowSpriteParts(struct GloveMotion *, void *);
void ActorParticles_SpawnImpactBurst(struct GloveMotion *);
void CharacterModel_UpdateAccessoryAnimations(void *);
void ScenePool_ClampMotionSpeeds(struct GloveMotion *);
void Actor_ClampMovementSpeed(struct GloveMotion *);
void CoffinBike_SetAnimationKeyframes(struct Glove *, s32);
void CoffinBike_UpdateMotion(struct Glove *);

void ActorParticles_UpdateEmitter(void *, void *, u32);
// confidence=low: message command one selects this recoil animation; stagger role is tentative.
void CoffinBike_UpdateStaggerRecoil(struct Glove *p)
{
    struct GloveMotion *self = &p->motion;
    s32 h, dx, dz;
    u32 a, b;
    if (p->active) {
        CoffinBike_SetAnimationKeyframes(p, 0x21);
        Trigger_ConsumePending(&p->queue, 1);
        p->active = 0;
    }
    {
        u16 bit = 0x10;
        p->flags |= bit;
    }
    CoffinBike_UpdateMotion(p);
    ScenePool_ClampMotionSpeeds(self);
    h = self->vy + self->unkC0;
    if (h > 0xF000)
        h = 0xF000;

    self->y += 0x200;
    if (self->steps > 0) {
        self->x += self->dx;
        self->dx += Div(-self->dx, self->steps);
        self->dy += Div(-self->dy, self->steps);
        self->dz += Div(-self->dz, self->steps);
        self->steps--;
    } else {
        Memory_Clear(&self->dx, 0x10);
    }
    Actor_ClampMovementSpeed(self);

    {
        s32 *pprevZ, *pprevX, *pvz, *px, *py;
        s32 lim;
        self->outX = h + self->x - self->previousPosition.x;
        self->outY = self->y + self->dy;
        self->outZ = self->vz + self->dz;
        dx = (self->x - self->previousPosition.x) >> 12;
        dz = (self->position.z - self->previousPosition.z) >> 12;
        lim = 0x3000;
        px = &self->x;
        pprevX = &self->previousPosition.x;
        py = &self->y;
        pprevZ = &self->previousPosition.z;
        pvz = &self->vz;
        if (h > lim)
            dx = 0x80;
        if (dx == 0 && dz == 0)
            self->angle = 0xFFFF;
        else
            self->angle = (Math_Atan2(dx, dz) >> 5) << 5;
        *pprevX = *px;
        *pprevZ = self->position.z;
        Terrain_ResolveActorMotion(self);
        a = (u32) ~(self->unk3C >> 12) >> 31;
        b = (u32) ~(self->unk38 >> 12) >> 31;
        if (self->unk4 != self->unk8 && (self->unk4 & 0x1000) && !(self->unk8 & 0xF000) && b <= 1 &&
            a == 1) {
            s32 v = ((self->vy >> 10) + (self->unkC0 >> 8)) >> 1;
            if (v < 4)
                v = 4;
            if (v > 18)
                v = 18;
            *py -= v << 9;
            self->flags40 &= ~1;
        }
        p->height = h;
        PalFade_Update(self);

        Actor_UpdateShadowSpriteParts(self, &p->model);
        if (self->flags40 & 4)
            ActorParticles_SpawnImpactBurst(self);
        if (self->flags40 & 2)
            *pvz = 0;
        if (self->flags40 & 1)
            *py = 0;
    }

    CharacterModel_UpdateAccessoryAnimations(&p->model);
    ActorParticles_UpdateEmitter(self, &self->emitter, self->style.initialWord);
    p->timer++;
}
