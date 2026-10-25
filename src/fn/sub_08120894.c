#include "global.h"

/* tentative: knockback fall state installed by the command dispatcher */

/* tentative: knockback/fall state; decays velocity, updates heading, on landing switches to ActorPhysics_UpdateFallMotion */

struct Vec4i {
    s32 x, y, z, w;
};

struct Obj {
    u8 pad0[4];
    u32 flagsA, flagsB;
    u8 pad0C[0x18 - 0xC];
    s32 unk18;
    u8 pad1C[4];
    s32 x, y, z;
    u8 pad2C[0x38 - 0x2C];
    s32 unk38, unk3C;
    s32 hit;
    u8 pad44[0xC0 - 0x44];
    s32 unkC0;
    u8 padC4[0x5AC - 0xC4];
    s32 unk5AC;
    s32 unk5B0;
    s32 unk5B4;
    u8 pad5B8[4];
    s32 unk5BC;
    u8 pad5C0[4];
    s32 unk5C4;
    u8 pad5C8[4];
    struct Vec4i vel;
    s32 unk5DC;
    u8 pad5E0[0x5EC - 0x5E0];
    s8 unk5EC;
    u8 pad5ED[3];
    u16 angle;
    u8 pad5F2[0x5F8 - 0x5F2];
    u8 model[0x84C - 0x5F8];
    u32 timer;
    u8 state;
    u8 pad851[3];
    void *fn;
    u8 queue[0x8AC - 0x858];
    s32 unk8AC;
    u8 pad8B0[0x8BA - 0x8B0];
    u16 unk8BA;
};

void RegistryA8CB_SetKeyframes(struct Obj *, s32);
s32 ActorMotion_UpdateDampedFall(struct Obj *);
s32 Div(s32, s32);
void Memory_Clear(void *, u32);
s32 Math_Atan2(s32, s32);
void Terrain_ResolveActorMotion(struct Obj *);
void Actor_UpdateShadowSpriteParts(struct Obj *, u8 *);
void ActorParticles_SpawnImpactBurst(struct Obj *);
void CharacterModel_UpdateAccessoryAnimations(u8 *);
void Trigger_ConsumePending(void *, s32);
void ActorPhysics_UpdateFallMotion(void);

static inline void SetState(struct Obj *p, void *fn)
{
    p->state = 1;
    p->fn = fn;
    p->timer = 0;
}

void SceneActor_UpdateKnockbackFall(struct Obj *p)
{
    struct Obj *self = p;
    u8 done;
    s32 h;
    s32 dx, dz;
    s32 a, b;
    s32 lim;
    s32 *prev, *cur, *py, *pz, *prevZ;

    if (p->state) {
        RegistryA8CB_SetKeyframes(p, 1);
        p->state = 0;
    }
    {
        u16 bit = 0x10;
        p->unk8BA |= bit;
    }
    done = ActorMotion_UpdateDampedFall(p) != 0;
    h = p->unk5AC + p->unkC0;
    p->unk5B0 += 0x200;
    if (p->unk5EC > 0) {
        p->unk5DC += p->vel.x;
        p->vel.x += Div(-p->vel.x, p->unk5EC);
        p->vel.y += Div(-p->vel.y, p->unk5EC);
        p->vel.z += Div(-p->vel.z, p->unk5EC);
        p->unk5EC--;
    } else {
        Memory_Clear(&p->vel, 0x10);
    }
    self->x = h + self->unk5DC - self->unk5BC;
    self->y = self->unk5B0 + self->vel.y;
    self->z = self->unk5B4 + self->vel.z;
    dx = (self->unk5DC - self->unk5BC) >> 12;
    dz = (self->unk18 - self->unk5C4) >> 12;
    lim = 0x3000;
    cur = &self->unk5DC;
    prev = &self->unk5BC;
    py = &self->unk5B0;
    pz = &self->unk5B4;
    prevZ = &self->unk5C4;
    if (h > lim)
        dx = 0x80;
    if (dx == 0 && dz == 0)
        self->angle = 0xFFFF;
    else
        self->angle = (Math_Atan2(dx, dz) >> 5) << 5;
    *prev = *cur;
    *prevZ = self->unk18;
    Terrain_ResolveActorMotion(self);
    a = (u32) ~(self->unk3C >> 12) >> 31;
    b = (u32) ~(self->unk38 >> 12) >> 31;
    if (self->flagsA != self->flagsB && (self->flagsA & 0x1000) && !(self->flagsB & 0xF000) &&
        (u32)b <= 1 && a == 1) {
        s32 t = ((self->unk5AC >> 10) + (self->unkC0 >> 8)) >> 1;
        if (t < 4)
            t = 4;
        if (t > 18)
            t = 18;
        *py -= t << 9;
        self->hit &= ~1;
    }
    p->unk8AC = h;
    Actor_UpdateShadowSpriteParts(self, p->model);
    if (self->hit & 4)
        ActorParticles_SpawnImpactBurst(self);
    if (self->hit & 2)
        *pz = 0;
    if (self->hit & 1)
        *py = 0;
    CharacterModel_UpdateAccessoryAnimations(p->model);
    if (done == 1) {
        Trigger_ConsumePending(p->queue, 1);
        SetState(p, ActorPhysics_UpdateFallMotion);
    } else {
        p->timer++;
    }
}
