#include "global.h"
#include "scene/actor/scene_actor_style.h"
#include "system/game_state/game_state.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "scene/areas/area_effect_link.h"
#include "engine/engine_pause.h"
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
void MotionActor_UpdateMovementAudio(struct Glove *);
u8 LinkParticipant_IsLocallySelected(struct Glove *);
void PalFade_Start(void *, s32, u16, s32, s32, s32, s32, s32);
s32 CharacterModel_GetPrimaryPaletteIndex(void *);
void ChargeMarker_TickHold(void *, void *, u32);
void CoffinBike_UpdateStopping(struct Glove *);

void GameState_AddActionCount894(s32);
void CharacterModel_ClearAnimationFlags(void *, s32);
void CharacterModel_SetAnimationFlags(void *, s32);
void ActorMarkerPool_ShowMarker(s32);
void Character_SetLowResourceColorEffect(void *, void *);

// confidence=low: restores appearance and resources after defeat recovery.
#define Glove_UpdateRecoveryFlash sub_08117F6C
union GloveResourceHalfword {
    u16 bits;
    s16 value;
};
static inline void Glove_ChangeUpdate(struct Glove *p, void (*callback)(void *))
{
    u8 *active = &p->active;
    *active = 1;
    p->update = callback;
    p->timer = 0;
}
void Glove_UpdateRecoveryFlash(struct Glove *p)
{
    struct GloveMotion *self = &p->motion;
    s32 h;
    s32 dx;
    s32 dz;
    u32 a;
    u32 b;
    u16 *flags;
    s32 *vy, *py, *pvz, *bias;
    if (p->active) {
        if (!Engine_PauseBits(0x800))
            GameState_AddActionCount894(1);
        CharacterModel_ClearAnimationFlags(&p->model, 1);
        p->active = 0;
        p->blinkTimer = 3;
    }
    {
        u16 bit = 1;
        p->flags |= bit;
    }
    {
        s32 decay;
        s32 *px;
        s32 *idy;
        s32 *idz;
        h = self->vy + self->unkC0;
        {
            s32 limit = 0xF000;
            flags = &p->flags;
            vy = &self->vy;
            bias = &self->unkC0;
            if (h > limit)
                h = limit;
        }
        self->y += 0x200;
        decay = self->steps;
        py = &self->y;
        if (decay > 0) {
            self->x += self->dx;
            self->dx += Div(-self->dx, self->steps);
            self->dy += Div(-self->dy, self->steps);
            self->dz += Div(-self->dz, self->steps);
            self->steps--;
            px = &self->x;
            idy = &self->dy;
            idz = &self->dz;
        } else {
            Memory_Clear(&self->dx, 0x10);
            px = &self->x;
            idy = &self->dy;
            idz = &self->dz;
        }
        Actor_ClampMovementSpeed(self);
        {
            s32 *pprevZ;
            s32 *pprevX;
            s32 lim;
            self->outX = ((*px) + h) - self->previousPosition.x;
            self->outY = (*py) + (*idy);
            self->outZ = self->vz + (*idz);
            dx = ((*px) - self->previousPosition.x) >> 12;
            dz = (self->position.z - self->previousPosition.z) >> 12;
            lim = 0x3000;
            pprevX = &self->previousPosition.x;
            pvz = &self->vz;
            pprevZ = &self->previousPosition.z;
            if (h > lim)
                dx = 0x80;
            if ((dx == 0) && (dz == 0))
                self->angle = 0xFFFF;
            else
                self->angle = (Math_Atan2(dx, dz) >> 5) << 5;
            *pprevX = *px;
            *pprevZ = self->position.z;
            Terrain_ResolveActorMotion(self);
            a = ((u32)(~(self->unk3C >> 12))) >> 31;
            b = ((u32)(~(self->unk38 >> 12))) >> 31;
            if (((((self->unk4 != self->unk8) && (self->unk4 & 0x1000)) &&
                  (!(self->unk8 & 0xF000))) &&
                 (b <= 1)) &&
                (a == 1)) {
                s32 v = (((*vy) >> 10) + ((*bias) >> 8)) >> 1;
                if (v < 4)
                    v = 4;
                if (v > 18)
                    v = 18;
                *py -= v << 9;
                self->flags40 &= ~1;
            }
            p->height = h;
            Actor_UpdateShadowSpriteParts(self, &p->model);
            if (self->flags40 & 2)
                *pvz = 0;
            if (self->flags40 & 1)
                *py = 0;
        }
    }
    if (p->blinkTimer) {
        p->blinkTimer--;
    } else {
        if ((p->model.parts[0].flags) & 1)
            CharacterModel_ClearAnimationFlags(&p->model, 1);
        else
            CharacterModel_SetAnimationFlags(&p->model, 1);
        p->blinkTimer = 3;
    }
    self->style.initialWord = self->style.currentWord;
    {
        union GloveResourceHalfword *current, *target;
        union GloveResourceHalfword *currentField = (void *)&self->style.initialHalfword;
        union GloveResourceHalfword *targetField = (void *)&self->style.currentHalfword;
        u32 currentBits = currentField->bits;
        s32 currentValue = (s16)currentBits;
        u32 targetBits = targetField->bits;
        s32 targetValue = (s16)targetBits;
        current = currentField;
        target = targetField;
        if (currentValue < targetValue) {
            if (target->value - current->value > 100)
                current->bits = currentBits + 20;
            else
                current->bits = currentBits + 5;
        } else
            current->bits = targetBits;
        if (p->timer > 120) {
            self->style.initialWord = self->style.currentWord;
            current->bits = target->bits;
            CharacterModel_ClearAnimationFlags(&p->model, 1);
            Glove_ChangeUpdate(p, (void (*)(void *))CoffinBike_UpdateStopping);
            ActorMarkerPool_ShowMarker(p->marker);
            {
                s32 mask = ~1;
                *flags &= mask;
            }
            Character_SetLowResourceColorEffect(&self->emitter, &self->style);
        } else
            p->timer++;
    }
}
