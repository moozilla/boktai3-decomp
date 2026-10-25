#include "global.h"

/* tentative: per-frame damped fall with offset decay and facing update */

struct Actor {
    u8 pad0[4];
    u32 state;     /* 0x04 */
    u32 prevState; /* 0x08 */
    u8 pad0C[0x0C];
    s32 z; /* 0x18 */
    u8 pad1C[4];
    s32 px, py, pz; /* 0x20 */
    u8 pad2C[0x0C];
    s32 v38, v3C; /* 0x38 */
    u32 flags;    /* 0x40 */
    u8 pad44[0xC0 - 0x44];
    s32 unkC0; /* 0xC0 */
    u8 padC4[0x5AC - 0xC4];
    s32 baseX; /* 0x5AC */
    s32 baseY; /* 0x5B0 */
    s32 baseZ; /* 0x5B4 */
    u8 pad5B8[4];
    s32 prevX; /* 0x5BC */
    s32 prevY;
    s32 prevZ; /* 0x5C4 */
    u8 pad5C8[4];
    s32 velX; /* 0x5CC */
    s32 velY;
    s32 velZ;
    s32 velW;
    s32 offX; /* 0x5DC */
    u8 pad5E0[0x5EC - 0x5E0];
    s8 decay; /* 0x5EC */
    u8 pad5ED[3];
    u16 angle; /* 0x5F0 */
    u8 pad5F2[6];
    u8 model[0x84C - 0x5F8];
    u32 timer;    /* 0x84C */
    u8 resetKeys; /* 0x850 */
    u8 pad851[0x8AC - 0x851];
    s32 lastX; /* 0x8AC */
    u8 pad8B0[0x8BA - 0x8B0];
    u16 flags8BA; /* 0x8BA */
};

void RegistryA8CB_SetKeyframes(struct Actor *, s32);
s32 ActorMotion_UpdateDampedFall(struct Actor *);
s32 Div(s32, s32);
void Memory_Clear(void *, u32);
s32 Math_Atan2(s32, s32);
void Terrain_ResolveActorMotion(struct Actor *);
void PalFade_Update(struct Actor *);
void Actor_UpdateShadowSpriteParts(struct Actor *, u8 *);
void ActorParticles_SpawnImpactBurst(struct Actor *);
void CharacterModel_UpdateAccessoryAnimations(u8 *);

void ActorMotion_UpdateFallFrame(struct Actor *p)
{
    struct Actor *self = p;
    s32 x;
    s32 dx, dz;
    u32 a, b;
    s32 lim;
    s32 *prevX, *offX, *baseY, *baseZ, *prevZ;

    if (p->resetKeys) {
        RegistryA8CB_SetKeyframes(p, 1);
        p->resetKeys = 0;
    }
    {
        u16 bit = 0x10;
        p->flags8BA |= bit;
    }
    ActorMotion_UpdateDampedFall(p);
    x = p->baseX + p->unkC0;
    p->baseY += 0x200;
    if (p->decay > 0) {
        p->offX += p->velX;
        p->velX += Div(-p->velX, p->decay);
        p->velY += Div(-p->velY, p->decay);
        p->velZ += Div(-p->velZ, p->decay);
        p->decay--;
    } else {
        Memory_Clear(&p->velX, 16);
    }
    self->px = x + self->offX - self->prevX;
    self->py = self->baseY + self->velY;
    self->pz = self->baseZ + self->velZ;
    dx = (self->offX - self->prevX) >> 12;
    dz = (self->z - self->prevZ) >> 12;
    lim = 0x3000;
    offX = &self->offX;
    prevX = &self->prevX;
    baseY = &self->baseY;
    baseZ = &self->baseZ;
    prevZ = &self->prevZ;
    if (x > lim)
        dx = 0x80;
    if (dx == 0 && dz == 0)
        self->angle = 0xFFFF;
    else
        self->angle = (Math_Atan2(dx, dz) >> 5) << 5;
    *prevX = *offX;
    *prevZ = self->z;
    Terrain_ResolveActorMotion(self);
    a = (u32) ~(self->v3C >> 12) >> 31;
    b = (u32) ~(self->v38 >> 12) >> 31;
    if (self->state != self->prevState && (self->state & 0x1000) && !(self->prevState & 0xF000) &&
        b <= 1 && a == 1) {
        s32 v = ((self->baseX >> 10) + (self->unkC0 >> 8)) >> 1;
        if (v < 4)
            v = 4;
        if (v > 18)
            v = 18;
        *baseY -= v << 9;
        self->flags &= ~1;
    }
    p->lastX = x;
    PalFade_Update(self);
    Actor_UpdateShadowSpriteParts(self, p->model);
    if (self->flags & 4)
        ActorParticles_SpawnImpactBurst(self);
    if (self->flags & 2)
        *baseZ = 0;
    if (self->flags & 1)
        *baseY = 0;
    CharacterModel_UpdateAccessoryAnimations(p->model);
    p->timer++;
}
