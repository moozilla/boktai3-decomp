#include "global.h"
#include "game_state.h"
#include "ram.h"
#include "engine_globals.h"
#include "area_effect_link.h"
// MATCHING_CONCESSION: register-pinning

#ifdef NONMATCHING
// fuzzy: 91.0%
// fix.py best version (reprinted C)
struct Obj {
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
    u8 pad44[0x68 - 0x44];
    u32 pendingDamage;
    u8 pad6C[0x7E - 0x6C];
    u16 recoveryValue;
    u8 pad80[0xC0 - 0x80];
    s32 unkC0;
    u8 padC4[0x307 - 0xC4];
    u8 actionActive;
    u16 actionCooldown;
    u8 pad30A[0x319 - 0x30A];
    u8 actionKind;
    u8 pad31A[0x326 - 0x31A];
    u16 recoveryTimer, cooldown;
    u8 pad32A[0x57A - 0x32A];
    u8 damageTimer;
    u8 pad57B[0x5AC - 0x57B];
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
    u8 pad5ED;
    s8 damageFlags;
    u8 pad5EF;
    u16 angle; /* 0x5F0 */
    u8 pad5F2[6];
};

struct Glove {
    u8 pad0[0x18];
    struct Obj motion;
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
#define U8(p, o)  (*(u8 *)((u8 *)(p) + (o)))
#define S8(p, o)  (*(s8 *)((u8 *)(p) + (o)))
#define U16(p, o) (*(u16 *)((u8 *)(p) + (o)))
#define S16(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define U32(p, o) (*(u32 *)((u8 *)(p) + (o)))
#define S32(p, o) (*(s32 *)((u8 *)(p) + (o)))
s32 Div(s32, s32);
void Memory_Clear(void *, u32);
s32 Math_Atan2(s32, s32);
void Terrain_ResolveActorMotion(struct Obj *);
void PalFade_Update(struct Obj *);
void Actor_UpdateShadowSpriteParts(struct Obj *, void *);
void ActorParticles_SpawnImpactBurst(struct Obj *);
void CharacterModel_UpdateAccessoryAnimations(void *);
void ScenePool_ClampMotionSpeeds(struct Obj *);
void Actor_ClampMovementSpeed(struct Obj *);
void MotionActor_UpdateMovementAudio(struct Glove *);
u8 LinkParticipant_IsLocallySelected(struct Glove *);
void Sound_StopSong(s32);
void Anim_SetKeyframes(void *, void *, u16, s32, s32);
void Anim_Step(void *, void *);
void PalFade_Start(void *, s32, u16, s32, s32, s32, s32, s32);
s32 CharacterModel_GetPrimaryPaletteIndex(void *);
s32 sub_08249240(void *, s32, u32);
void ChargeMarker_TickHold(void *, void *, u32);
void Glove_UpdateStopping(struct Glove *);

// Action-pointer lifetimes, constant reuse and register allocation still differ.
static inline void ClearDamageFlag(s8 *p, s32 mask)
{
    *p &= mask;
}
static inline void UpdateSecondaryAction(struct Glove *p, u16 *flags)
{
    struct Obj *self = &p->motion;
    if (*((u8 *)(((u8 *)p) + 0x33C))) {
        if ((*((u8 *)(((u8 *)p) + 0x331))) == 5) {
            s8 palette = CharacterModel_GetPrimaryPaletteIndex(p->model);
            if (p->input[0] & 1) {
                if (!sub_08249240(((u8 *)p) + 0x32C, 0, *((u32 *)(((u8 *)p) + 0x32C)))) {
                    if ((*((u8 *)(((u8 *)p) + 0x343))) == 1) {
                        PalFade_Start(self, palette, *((u16 *)(((u8 *)p) + 0x96)), 0, 0, 16, 1, 1);
                        *((u8 *)(((u8 *)p) + 0x343)) = 0;
                    }
                    *((u8 *)(((u8 *)p) + 0x342)) = 0;
                }
            } else if ((*((u8 *)(((u8 *)p) + 0x343))) && (p->previousInput & 1)) {
                PalFade_Start(self, palette, *((u16 *)(((u8 *)p) + 0x96)), 0, 0, 16, 1, 1);
                *((u8 *)(((u8 *)p) + 0x342)) = 0;
            }
        } else if (p->input[1] & 1)
            sub_08249240(((u8 *)p) + 0x32C, 0, *((u32 *)(((u8 *)p) + 0x32C)));
        if (self->actionKind == 4) {
            if (self->recoveryTimer) {
                u16 bit = 2;
                *flags |= bit;
                self->recoveryTimer--;
            }
        } else if ((self->actionKind == 1) && self->recoveryTimer) {
            self->recoveryTimer--;
            ClearDamageFlag(&self->damageFlags, ~1);
            self->damageTimer = 0;
            self->pendingDamage = 0;
            if (!self->recoveryTimer) {
                s32 palette = CharacterModel_GetPrimaryPaletteIndex(p->model);
                PalFade_Start(self, palette, self->recoveryValue, 0, 0, 16, 1, 1);
            }
        }
        if (self->cooldown)
            self->cooldown--;
    }
}

static inline void UpdateActions(struct Glove *p, u16 *flags)
{
    struct Obj *self = &p->motion;
    if (U8(p, 0x2CD) == 2) {
        ChargeMarker_TickHold(self, p->model, (u32)p->input);
    } else if (U8(p, 0x2CD) == 5) {
        if (p->input[0] & 2) {
            if (!sub_08249240((u8 *)p + 0x2C8, 0, U32(p, 0x2C8))) {
                if (U8(p, 0x31E)) {
                    Anim_SetKeyframes((u8 *)p + 0x670, (u8 *)p + 0x7F0, U16(p, 0x326), 2, 0);
                    Anim_Step((u8 *)p + 0x670, (u8 *)p + 0x7F0);
                    U8(p, 0x31D) = 1;
                    U8(p, 0x31E) = 0;
                    if (LinkParticipant_IsLocallySelected(p))
                        Sound_StopSong(0x93);
                }
                self->actionActive = 1;
            } else {
                U8(p, 0x31E) = 1;
            }
        } else {
            if (U8(p, 0x31E)) {
                Anim_SetKeyframes((u8 *)p + 0x670, (u8 *)p + 0x7F0, U16(p, 0x326), 2, 0);
                Anim_Step((u8 *)p + 0x670, (u8 *)p + 0x7F0);
                U8(p, 0x31D) = 1;
                U8(p, 0x31E) = 0;
                if (LinkParticipant_IsLocallySelected(p))
                    Sound_StopSong(0x93);
            }
            self->actionActive = 0;
        }
    } else if (U8(p, 0x2CD) == 3) {
        if (p->input[1] & 2)
            sub_08249240((u8 *)p + 0x2C8, 0, U32(p, 0x2C8));
        if (U16(p, 0x320) == 1)
            Anim_SetKeyframes((u8 *)p + 0x670, (u8 *)p + 0x7F0, U16(p, 0x326), 2, 0);
        Anim_Step((u8 *)p + 0x670, (u8 *)p + 0x7F0);
    } else {
        if (p->input[1] & 2)
            sub_08249240((u8 *)p + 0x2C8, 0, U32(p, 0x2C8));
    }
    if (self->actionCooldown)
        self->actionCooldown--;
    UpdateSecondaryAction(p, flags);
    if ((!p || !(*flags & 4)) && (U32(self, 0xC) & 0x800))
        self->damageFlags |= 4;
}
void Glove_UpdateStopping(struct Glove *p)
{
    struct Obj *self = &p->motion;
    s32 h;
    s32 dx;
    s32 dz;
    u32 a;
    u32 b;
    {
        register u8 *entry asm("r4") = &p->active;
        if (*entry) {
            {
                typeof(*((u16 *)(((u8 *)p) + 0x53C))) *q1 = &(*((u16 *)(((u8 *)p) + 0x53C)));
                u8 z1 = 0;
                *q1 = 0x88;
                *((u16 *)(((u8 *)p) + 0x53E)) = z1;
                *entry = z1;
            }
        }
    }
    {
        u16 *flags = &p->flags;
        MotionActor_UpdateMovementAudio(p);
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
            s32 *pprevZ;
            s32 *pprevX;
            s32 *pvz;
            s32 *px;
            s32 *py;
            s32 lim;
            self->outX = (h + self->x) - self->prevX;
            self->outY = self->y + self->dy;
            self->outZ = self->vz + self->dz;
            dx = (self->x - self->prevX) >> 12;
            dz = (self->z - self->prevZ) >> 12;
            lim = 0x3000;
            px = &self->x;
            pprevX = &self->prevX;
            py = &self->y;
            pprevZ = &self->prevZ;
            pvz = &self->vz;
            if (h > lim)
                dx = 0x80;
            if ((dx == 0) && (dz == 0))
                self->angle = 0xFFFF;
            else
                self->angle = (Math_Atan2(dx, dz) >> 5) << 5;
            *pprevX = *px;
            *pprevZ = self->z;
            Terrain_ResolveActorMotion(self);
            a = ((u32)(~(self->unk3C >> 12))) >> 31;
            b = ((u32)(~(self->unk38 >> 12))) >> 31;
            if (((((self->unk4 != self->unk8) && (self->unk4 & 0x1000)) &&
                  (!(self->unk8 & 0xF000))) &&
                 (b <= 1)) &&
                (a == 1)) {
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
            Actor_UpdateShadowSpriteParts(self, p->model);
            if (self->flags40 & 4)
                ActorParticles_SpawnImpactBurst(self);
            if (self->flags40 & 2)
                *pvz = 0;
            if (self->flags40 & 1)
                *py = 0;
        }
        UpdateActions(p, flags);
    }
    CharacterModel_UpdateAccessoryAnimations(p->model);
    p->timer++;
}
#else
INCLUDE_ASM("asm/nonmatching", Glove_UpdateStopping);
#endif
