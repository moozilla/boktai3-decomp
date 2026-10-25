#include "global.h"

/* tentative: per-frame fall/ease state installed by the command dispatcher */

struct Obj {
    u8 pad0[4];
    u32 unk4, unk8;
    u8 padC[0xC];
    s32 unk18;
    u8 pad1C[4];
    s32 x, y, z; /* 0x20 */
    u8 pad2C[0xC];
    s32 unk38, unk3C;
    u32 flags40;
    u8 pad44[0xC0 - 0x44];
    s32 unkC0;
    u8 padC4[0x5AC - 0xC4];
    s32 velY; /* 0x5AC */
    s32 posY; /* 0x5B0 */
    s32 posZ; /* 0x5B4 */
    u8 pad5B8[4];
    s32 prevX; /* 0x5BC */
    u8 pad5C0[4];
    s32 prevZ; /* 0x5C4 */
    u8 pad5C8[4];
    s32 stepX; /* 0x5CC */
    s32 ofsY;  /* 0x5D0 */
    s32 ofsZ;  /* 0x5D4 */
    u8 pad5D8[4];
    s32 curX; /* 0x5DC */
    u8 pad5E0[0xC];
    s8 steps; /* 0x5EC */
    u8 pad5ED[3];
    u16 angle; /* 0x5F0 */
    u8 pad5F2[6];
    u8 model[0x84C - 0x5F8];
    u32 timer;
    u8 state;
    u8 pad851[7];
    u8 queue[0x8AC - 0x858];
    s32 unk8AC;
    u8 pad8B0[0x8BA - 0x8B0];
    u16 flags8BA;
};

void RegistryA8CB_SetKeyframes(void *, u32);
u32 Trigger_ConsumePending(void *, u32);
s32 ActorMotion_UpdateDampedFall(void *);
s32 Div(s32, s32);
void Memory_Clear(void *, s32);
s32 Math_Atan2(s32, s32);
void Terrain_ResolveActorMotion(struct Obj *);
void PalFade_Update(struct Obj *);
void Actor_UpdateShadowSpriteParts(struct Obj *, void *);
void ActorParticles_SpawnImpactBurst(struct Obj *);
void CharacterModel_UpdateAccessoryAnimations(u8 *);

void SceneActor_UpdateFallState(struct Obj *p)
{
    struct Obj *self = p;
    s32 sum;
    s32 dx, dz;
    u32 a, b;
    s32 lim;
    s32 *prevX, *curX, *posY, *posZ, *prevZ;

    if (p->state) {
        RegistryA8CB_SetKeyframes(p, 1);
        Trigger_ConsumePending(p->queue, 1);
        p->state = 0;
    }
    {
        u16 bit = 0x10;
        p->flags8BA |= bit;
    }
    ActorMotion_UpdateDampedFall(p);
    sum = p->velY + p->unkC0;
    p->posY += 0x200;
    if (p->steps > 0) {
        p->curX += p->stepX;
        p->stepX += Div(-p->stepX, p->steps);
        p->ofsY += Div(-p->ofsY, p->steps);
        p->ofsZ += Div(-p->ofsZ, p->steps);
        p->steps--;
    } else {
        Memory_Clear(&p->stepX, 0x10);
    }
    self->x = sum + self->curX - self->prevX;
    self->y = self->posY + self->ofsY;
    self->z = self->posZ + self->ofsZ;
    dx = (self->curX - self->prevX) >> 12;
    dz = (self->unk18 - self->prevZ) >> 12;
    lim = 0x3000;
    curX = &self->curX;
    prevX = &self->prevX;
    posY = &self->posY;
    posZ = &self->posZ;
    prevZ = &self->prevZ;
    if (sum > lim)
        dx = 0x80;
    if (dx == 0 && dz == 0)
        self->angle = 0xFFFF;
    else
        self->angle = Math_Atan2(dx, dz) >> 5 << 5;
    *prevX = *curX;
    *prevZ = self->unk18;
    Terrain_ResolveActorMotion(self);
    a = (u32) ~(self->unk3C >> 12) >> 31;
    b = (u32) ~(self->unk38 >> 12) >> 31;
    if (self->unk4 != self->unk8 && (self->unk4 & 0x1000) && !(self->unk8 & 0xF000) && b <= 1 &&
        a == 1) {
        s32 v = ((self->velY >> 10) + (self->unkC0 >> 8)) >> 1;
        if (v < 4)
            v = 4;
        if (v > 18)
            v = 18;
        *posY -= v << 9;
        self->flags40 &= ~1;
    }
    p->unk8AC = sum;
    PalFade_Update(self);
    Actor_UpdateShadowSpriteParts(self, p->model);
    if (self->flags40 & 4)
        ActorParticles_SpawnImpactBurst(self);
    if (self->flags40 & 2)
        *posZ = 0;
    if (self->flags40 & 1)
        *posY = 0;
    CharacterModel_UpdateAccessoryAnimations(p->model);
    p->timer++;
}
