#include "global.h"
#include "game_state.h"
#include "ram.h"
// MATCHING_CONCESSION: register-pinning

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
extern u8 *gUnk_020004B4;
void Glove_UpdateStopping(struct Glove *);

void GameState_AddActionCount894(s32);
void CharacterModel_ClearAnimationFlags(void *, s32);
void CharacterModel_SetAnimationFlags(void *, s32);
void ActorMarkerPool_ShowMarker(s32);
void Character_SetLowResourceColorEffect(void *, void *);

#ifdef NONMATCHING
// fuzzy: 82.3%
// fix.py best (sweep after the inlined-insn fix)
// confidence=low: restores appearance and resources after defeat recovery.
#define Glove_UpdateRecoveryFlash sub_08117F6C
static inline u32 LinkMode(u32 mask)
{
    return (gEngineFlags | gScenePauseFlags) & mask;
}
void Glove_UpdateRecoveryFlash(struct Glove *p)
{
    struct Obj *self = &p->motion;
    register s32 h asm("r10");
    s32 dx;
    s32 dz;
    u32 a;
    u32 b;
    u16 *flags;
    if (p->active) {
        if (!LinkMode(0x800))
            GameState_AddActionCount894(1);
        CharacterModel_ClearAnimationFlags(p->model, 1);
        p->active = 0;
        p->blinkTimer = 3;
    }
    {
        u16 bit = 1;
        flags = &p->flags;
        *flags |= bit;
    }
    {
        s32 *vy = &self->vy;
        s32 *bias = &self->unkC0;
        s32 *py;
        s32 *px;
        s32 *idy;
        s32 *idz;
        h = (*vy) + (*bias);
        if (h > 0xF000)
            h = 0xF000;
        self->y += 0x200;
        py = &self->y;
        if (self->steps > 0) {
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
            s32 *pvz;
            s32 lim;
            self->outX = ((*px) + h) - self->prevX;
            self->outY = (*py) + (*idy);
            self->outZ = self->vz + (*idz);
            dx = ((*px) - self->prevX) >> 12;
            dz = (self->z - self->prevZ) >> 12;
            lim = 0x3000;
            pprevX = &self->prevX;
            pvz = &self->vz;
            pprevZ = &self->prevZ;
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
                s32 v = (((*vy) >> 10) + ((*bias) >> 8)) >> 1;
                if (v < 4)
                    v = 4;
                if (v > 18)
                    v = 18;
                *py -= v << 9;
                self->flags40 &= ~1;
            }
            p->height = h;
            Actor_UpdateShadowSpriteParts(self, p->model);
            if (self->flags40 & 2)
                *pvz = 0;
            if (self->flags40 & 1)
                *py = 0;
        }
    }
    if (p->blinkTimer) {
        p->blinkTimer--;
    } else {
        if ((*((u32 *)(((u8 *)p) + 0x618))) & 1)
            CharacterModel_ClearAnimationFlags(p->model, 1);
        else
            CharacterModel_SetAnimationFlags(p->model, 1);
        p->blinkTimer = 3;
    }
    *((u32 *)(((u8 *)self) + 0x5C)) = *((u32 *)(((u8 *)self) + 0x60));
    {
        u16 *current = (u16 *)(((u8 *)self) + 0x58);
        u16 *target = (u16 *)(((u8 *)self) + 0x5A);
        if (((s16)(*current)) < ((s16)(*target))) {
            if ((((s16)(*target)) - (*((s16 *)current))) > 100)
                *current += 20;
            else
                *current += 5;
        } else
            *current = *target;
        if (p->timer > 120) {
            *((u32 *)(((u8 *)self) + 0x5C)) = *((u32 *)(((u8 *)self) + 0x60));
            *current = *target;
            CharacterModel_ClearAnimationFlags(p->model, 1);
            {
                void (*callback)(void *) = (void (*)(void *))Glove_UpdateStopping;
                u8 *active = &p->active;
                u32 zero = 0;
                *active = 1;
                p->update = callback;
                p->timer = zero;
            }
            ActorMarkerPool_ShowMarker(p->marker);
            {
                s32 mask = ~1;
                *flags &= mask;
            }
            Character_SetLowResourceColorEffect(((u8 *)self) + 0x44, ((u8 *)self) + 0x48);
        } else
            p->timer++;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08117F6C);
#endif
