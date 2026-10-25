#include "global.h"

/* tentative: damped fall with offset easing, facing and ground step */

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
    u8 model[0x84C - 0x5F8];
    s32 timer;    /* 0x84C */
    u8 keyframes; /* 0x850 */
    u8 pad851[0x8AC - 0x851];
    s32 height; /* 0x8AC */
    u8 pad8B0[0x8BA - 0x8B0];
    u16 flags8BA;
};

void RegistryA8CB_SetKeyframes(struct Obj *, s32);
s32 ActorMotion_UpdateDampedFall(struct Obj *);
s32 Div(s32, s32);
void Memory_Clear(void *, u32);
s32 Math_Atan2(s32, s32);
void Terrain_ResolveActorMotion(struct Obj *);
void Actor_UpdateShadowSpriteParts(struct Obj *, void *);
void ActorParticles_SpawnImpactBurst(struct Obj *);
void CharacterModel_UpdateAccessoryAnimations(void *);

void ActorPhysics_UpdateFallMotion(struct Obj *p)
{
    struct Obj *self = p;
    s32 h;
    s32 dx, dz;
    u32 a, b;

    if (p->keyframes) {
        RegistryA8CB_SetKeyframes(p, 1);
        p->keyframes = 0;
    }
    {
        u16 bit = 0x10;
        p->flags8BA |= bit;
    }
    ActorMotion_UpdateDampedFall(p);
    h = p->vy + p->unkC0;
    p->y += 0x200;
    if (p->steps > 0) {
        p->x += p->dx;
        p->dx += Div(-p->dx, p->steps);
        p->dy += Div(-p->dy, p->steps);
        p->dz += Div(-p->dz, p->steps);
        p->steps--;
    } else {
        Memory_Clear(&p->dx, 0x10);
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
        CharacterModel_UpdateAccessoryAnimations(p->model);
        p->timer++;
    }
}
