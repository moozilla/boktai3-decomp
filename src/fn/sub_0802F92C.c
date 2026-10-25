#include "global.h"

// confidence=low: steps the actor toward its probe target along the route, with table-driven motion when the animation allows.

// confidence=low: steps the actor toward its probe target along the route, with table-driven motion when the animation allows.

// confidence=low: steps the actor toward its probe target along the route, with table-driven motion when the animation allows.

// confidence=low: steps the actor toward its probe target along the route, with table-driven motion when the animation allows.

// confidence=low: steps the actor toward its probe target along the route, with table-driven motion when the animation allows.
extern const s16 gUnk_086149C4[];

struct Sub0802F92CVec {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
};

struct Sub0802F92CMotion {
    u8 unk00[9];
    u8 angle;
    u8 unk0A[2];
    struct Sub0802F92CVec pos;
};

struct Sub0802F92CProbe {
    u8 unk00[0xC];
    struct Sub0802F92CVec current;
    u8 unk14[8];
    struct Sub0802F92CVec target;
};

struct Sub0802F92CAnim {
    u8 unk00[0x14];
    u16 flags14;
    u8 unk16[0x1D - 0x16];
    u8 flags1D;
};

struct Sub0802F92CActor {
    u8 unk00[7];
    u8 flag07;
    u8 unk08;
    u8 flag09;
    u8 unk0A[4];
    u16 flags0E;
    u8 unk10[9];
    u8 unk19;
    u8 unk1A;
    u8 unk1B[0x5C - 0x1B];
    struct Sub0802F92CMotion motion;
    u8 unk70[0xD4 - 0x70];
    u32 animType;
    u8 unkD8[0x100 - 0xD8];
    struct Sub0802F92CAnim anim;
    u8 unk11E[0x19C - 0x11E];
    u8 route[8];
    u32 pathSource;
    struct Sub0802F92CProbe probe;
    u8 unk1CC[0x1F0 - 0x1CC];
    u8 clear1F0;
    u8 unk1F1[2];
    s8 routeParam1F3;
    u8 unk1F4[0x210 - 0x1F4];
    u16 routeParam210;
    u8 unk212[0x214 - 0x212];
    u16 routeParam214;
    u8 unk216[0x2E4 - 0x216];
    void *callback;
};

static inline s32 Helper0802F92C_Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline void Helper0802F92C_Polar(struct Sub0802F92CVec *out, u8 angle, s32 speed)
{
    out->x = Helper0802F92C_Scale(speed * gUnk_086149C4[(u8)(angle + 0x40)]);
    out->y = 0;
    out->z = Helper0802F92C_Scale(speed * gUnk_086149C4[angle]);
}

static inline void Helper0802F92C_Aim(struct Sub0802F92CActor *self)
{
    struct Sub0802F92CProbe *probe = &self->probe;
    struct Sub0802F92CMotion *motion = &self->motion;
    s32 angle = Math_Atan2(probe->target.x - motion->pos.x, probe->target.z - motion->pos.z);
    u16 narrowed;
    s32 value;

    motion->angle = angle;
    narrowed = angle;
    value = narrowed;
    self->unk1A = ((value + 0x70) >> 5) & 7;
    self->unk19 = ((value + 0x60) >> 6) & 3;
}

#ifdef NONMATCHING
// fuzzy: 36.6%
// scope shape 106 (differences once low registers are set aside)
// confidence=low: steps the actor toward its probe target along the route, with table-driven motion when the animation allows.
void sub_0802F92C(void *unused, struct Sub0802F92CActor *self)
{
    struct Sub0802F92CMotion *motion = &self->motion;
    struct Sub0802F92CProbe *probe = &self->probe;
    struct Sub0802F92CVec *pos = &motion->pos;
    struct Sub0802F92CVec path;
    struct Sub0802F92CVec step;
    s32 dx;
    s32 dz;
    s32 distSq;

    if (self->flag09) {
        self->flag09 = 0;
        self->flag07 = 0;
        self->clear1F0 = 0;
        ProximityVolume_TranslateBounds(probe, pos);
        Route_Start(self->route, self->routeParam1F3, self->routeParam210, self->routeParam214);
        Path_GetPointPosition(&path, (void *)self->pathSource, 0);
        Terrain_ApplyBlocked(probe, pos, &path);
        Helper0802F92C_Aim(self);
        Actor_PlayAnimationEntry(self, 1, 1, 0, 4);
    }
    self->flags0E |= 0x1000;
    if ((u8)Callback_InvokeActor(self, self->callback) != 1) {
        dx = probe->current.x - motion->pos.x;
        dz = probe->current.z - pos->z;
        distSq = dx * dx + dz * dz;
        if (distSq <= 0x9C3) {
            motion->pos.x = probe->target.x;
            motion->pos.z = probe->target.z;
            self->flag07 = 1;
        } else if (self->animType == 2) {
            struct Sub0802F92CAnim *anim = &self->anim;
            if (anim->flags1D & 4) {
                s32 speed;
                Helper0802F92C_Aim(self);
                speed = (anim->flags14 & 1) ? 0x18 : 0x30;
                Helper0802F92C_Polar(&step, motion->angle, speed);
                motion->pos.x += step.x;
                motion->pos.z += step.z;
            }
        }
        NavMesh_UpdateProbe(probe, pos);
        Actor_PlayFootstepSound(self, 1);
        Actor_PlayAnimationEntry(self, 1, 1, 0, 1);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0802F92C);
#endif
