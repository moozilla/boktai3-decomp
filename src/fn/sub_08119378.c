#include "global.h"
#include "game_state.h"
#include "ram.h"

// confidence=low: recoil completion returns to the callback at 081198F8.

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
    u8 pad44[0xC0 - 0x44];
    s32 unkC0;
    u8 padC4[0x5AC - 0xC4];
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
void Glove_SetAnimationKeyframes(struct Glove *, s32);
s32 ActorRecoil_UpdateMotion(struct Glove *);
void Actor_UpdateShadowSpriteParts(struct Obj *, void *);
void Glove_UpdateRestingMotion(void *);
extern u8 *gUnk_020004B4;

void ActorParticles_UpdateEmitter(void *, void *, u32);
void Trigger_ConsumePending(void *, s32);
// Animation zero plays until recoil completes, then returns to its resting callback.
void Glove_UpdateRecoilReturn(struct Glove *p)
{
    struct Obj *self = &p->motion;
    u8 complete;
    s32 h, dx, dz;
    u32 a, b;
    if (p->active) {
        Glove_SetAnimationKeyframes(p, 0);
        p->active = 0;
    }
    {
        u16 bit = 0x10;
        p->flags |= bit;
    }
    complete = ActorRecoil_UpdateMotion(p) != 0;
    h = self->vy + self->unkC0;

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

    {
        s32 *pprevZ, *pprevX, *pvz, *px, *py;
        s32 lim;
        self->outX = h + self->x - self->prevX;
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
        if (dx == 0 && dz == 0)
            self->angle = 0xFFFF;
        else
            self->angle = (Math_Atan2(dx, dz) >> 5) << 5;
        *pprevX = *px;
        *pprevZ = self->z;
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

        Actor_UpdateShadowSpriteParts(self, p->model);
        if (self->flags40 & 4)
            ActorParticles_SpawnImpactBurst(self);
        if (self->flags40 & 2)
            *pvz = 0;
        if (self->flags40 & 1)
            *py = 0;
    }

    CharacterModel_UpdateAccessoryAnimations(p->model);
    ActorParticles_UpdateEmitter(self, (u8 *)self + 0x44, U32(self, 0x5C));
    if (complete == 1) {
        Trigger_ConsumePending((u8 *)p + 0x870, 1);
        {
            void (*callback)(void *) = Glove_UpdateRestingMotion;
            u8 *active = &p->active;
            u32 zero = 0;
            *active = complete;
            p->update = callback;
            p->timer = zero;
        }
    } else
        p->timer++;
}
