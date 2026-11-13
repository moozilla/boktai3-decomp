#include "global.h"
#include "ram.h"
#include "background_controller.h"
#include "scene_object.h"
#include "game_state.h"

void Actor_ClampMovementSpeed(u8 *s)
{
    s32 *p = (s32 *)(s + 0x5dc);
    if (*p > 0xc8000)
        *p = 0xc8000;
    if (*p <= 0x1ffff)
        *p = 0x20000;
}

struct E {
    u8 pad[0x24];
    s16 v;
    u8 pad2[0x3A];
};

struct S {
    struct E e[5];
};

extern s8 gUnk_086106C0[];

void Actor_SetSpriteDepthOffsets(struct S *p, s32 d)
{
    s32 i;
    for (i = 0; i < 3; i++)
        p->e[i].v = gUnk_086106C0[i] + d;
    p->e[3].v = gUnk_086106C0[3] + d;
    p->e[4].v = gUnk_086106C0[3] + d;
}

/* tentative: positions sprite parts, easing two toward terrain clearance */

struct H {
    u32 lo : 16;
    u32 hi : 16;
};

struct Z {
    struct H a, b;
};

struct Vec {
    s16 x, y, z;
};

struct Part {
    u8 pad[0x20];
    s16 x, y, z;
    u8 pad2[0x3A];
};

struct Sprite {
    struct Part e[5];
};

struct Actor {
    u8 pad[0x10];
    s32 x, y, z;
    u8 pad1[0x38 - 0x1C];
    s32 yOff;
    u8 pad2[0x5AC - 0x3C];
    s32 v5AC;
};

struct Cam {
    u8 pad[0x44];
    s32 x;
};

extern const u16 gUnk_086141BC[];

s32 Terrain_GetCellClearance(struct Z *);

void Anim_SetFrameDurationScale(void *, u32);

void Actor_SetSpriteDepthOffsets();

static inline int Rand(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3FF;
    return gUnk_086141BC[gEffectRandomTableIndex];
}

void Actor_UpdateShadowSpriteParts(struct Actor *a, struct Sprite *s)
{
    struct Vec pos;
    struct Z tmp;
    s32 i;
    s32 h;
    s32 v;
    s16 *out;

    out = &pos.x;
    pos.x = (a->x >> 12) - ((struct Cam *)gSceneCamera)->x;
    out[1] = ((a->z + (a->y + a->yOff)) >> 12) + 0x5A;
    out[2] = a->z >> 12;
    for (i = 0; i < 3; i++) {
        s->e[i].x = pos.x;
        s->e[i].y = pos.y;
    }
    for (i = 3; i <= 4; i++) {
        s32 ax = a->x >> 12;
        pos.x = ax - ((struct Cam *)gSceneCamera)->x;
        tmp.a.hi = a->y >> 12;
        tmp.b.lo = a->z >> 12;
        switch (i) {
            case 3:
                pos.x += 15;
                pos.y -= 2;
                tmp.a.lo = ax + 15;
                break;
            case 4:
                pos.x -= 19;
                pos.y -= 2;
                tmp.a.lo = ax - 19;
                break;
        }
        h = Terrain_GetCellClearance(&tmp) - 6;
        if (pos.y - h > 8)
            do {
                h = pos.y - 8;
            } while (0);
        else if (pos.y - h < -8)
            do {
                h = pos.y + 8;
            } while (0);
        s->e[i].x = pos.x;
        if (s->e[i].y + 3 < h)
            s->e[i].y += 3;
        else if (s->e[i].y - 3 > h)
            s->e[i].y -= 3;
        else
            s->e[i].y = h;
        s->e[i].z = (a->z >> 12) - 6;
        if ((Rand() & 15) > 9) {
            s->e[i].y += (Rand() & 3) - 1;
        }
    }
    v = (a->v5AC >> 12) + 1;
    if (v != 1) {
        s32 sc = 0x40 - v * 2;
        sc = sc < 0x20 ? 0x20 : sc;
        Anim_SetFrameDurationScale(&s->e[3], (u16)sc);
        Anim_SetFrameDurationScale(&s->e[4], (u16)sc);
    }
    Actor_SetSpriteDepthOffsets(s, a->z >> 12);
}

u32 ActorEffect_SpawnAtActorPos();

static inline u32 NextRandom(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    return gUnk_086141BC[gEffectRandomTableIndex];
}

static inline u8 HasModifier(u32 mask)
{
    if (gGameState->flags868 & mask)
        return 1;
    return 0;
}

/* Tentative: emit paired particles when the actor reports an impact. */

void ActorParticles_SpawnImpactBurst(void *actor)
{
    struct SceneExtent offset, velocity;
    s32 i;
    u8 modifier;
    Sound_PlaySong(0x90);
    Memory_Clear(&offset, 8);
    modifier = HasModifier(8);
    for (i = 0; i < 4; i++) {
        offset.x = (NextRandom() & 31) - 7;
        velocity.x = (NextRandom() & 3) - 2;
        velocity.y = -(NextRandom() & 1);
        velocity.z = 0;
        ActorEffect_SpawnAtActorPos(actor, &offset, &velocity, 0x409C, 0x1C22, 4, 0, modifier, 0,
                                    0xE3, 30, 0, 512, 1, 0);
        ActorEffect_SpawnAtActorPos(actor, &offset, &velocity, 0x409C, 0x1C22, 3, 0, modifier, 0,
                                    0xE3, 12, 0, 1024, 1, 0);
    }
}

#define gRandomValues gUnk_086141BC

u32 ActorParticles_CreateAtOffset();

static inline u16 NextOffsetRandom(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3ff;
    return gUnk_086141BC[gEffectRandomTableIndex];
}

void ActorParticles_EmitOffsetBurst(void *actor)
{
    s32 i = 0;
    struct SceneExtent offset;
    do {
        offset.x = (NextOffsetRandom() & 31) - 7;
        offset.y = -(NextOffsetRandom() & 31);
        offset.z = 0;
        ActorParticles_CreateAtOffset(actor, &offset, 0, 0x8639, 0x75f6, 0, 0, 0, 0, 0x153, 30, 0,
                                      0, 1, 0);
    } while (++i < 4);
}

/* tentative: spawns a jittered pair of particles moving left and right */

struct ParticleOffset {
    s32 x : 16;
    s32 y : 16;
    s32 z : 16;
    s32 unused : 16;
};

struct SideBurstActor {
    u8 pad0[0x4a];
    u8 field4A;
    u8 field4B;
};

u32 ActorEffect_SpawnAtActorPos(struct SideBurstActor *, struct ParticleOffset *, struct ParticleOffset *,
                                u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32, u32);

static inline u32 ParticleRandom(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 1023;
    return gUnk_086141BC[gEffectRandomTableIndex];
}

/* Spawns two 0x3D99 particles from the actor: a shared jittered base offset
   (y = -32) and two velocity offsets, one leftward then one rightward (tentative). */

void ActorParticles_SpawnSideBurst(struct SideBurstActor *p)
{
    struct ParticleOffset a, b;
    u32 anim = p->field4B + 0x378;
    a.x = (ParticleRandom() & 31) - 7;
    a.y = -32;
    a.z = 0;
    b.x = -(ParticleRandom() & 3) - 3;
    b.y = -(ParticleRandom() & 7) - 3;
    b.z = (ParticleRandom() & 7) - 3;
    {
        u8 layer = p->field4A;
        ActorEffect_SpawnAtActorPos(p, &a, &b, 0x3d99, 0, layer, 1, 0, 0, anim, 0x30, 1, 0, 2, 6);
    }
    b.x = (ParticleRandom() & 3) + 2;
    b.y = -(ParticleRandom() & 7) - 2;
    b.z = (ParticleRandom() & 7) - 3;
    {
        u8 layer = p->field4A;
        ActorEffect_SpawnAtActorPos(p, &a, &b, 0x3d99, 0, layer, 1, 0, 0, anim, 0x30, 1, 0, 2, 6);
    }
}

struct ActorParticleEmitter {
    u8 interval, countdown, count, enabled;
};

u32 ActorParticles_CreateAtOffset(void *, struct SceneExtent *, struct SceneExtent *, u32, u32, u32,
                                  u32, u32, u32, u32, u32, u32, u32, u32, u32);

static inline u32 NextEmitterRandom(void)
{
    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3ff;
    return gUnk_086141BC[gEffectRandomTableIndex];
}

void ActorParticles_UpdateEmitter(void *actor, struct ActorParticleEmitter *emitter)
{
    struct SceneExtent offset, velocity;
    s32 i;
    if (!emitter->enabled)
        return;
    if (emitter->countdown) {
        emitter->countdown--;
        return;
    }
    emitter->countdown = emitter->interval;
    for (i = 0; i < emitter->count; i++) {
        offset.x = (NextEmitterRandom() & 31) - 7;
        offset.y = 0;
        offset.z = 0;
        velocity.x = (NextEmitterRandom() & 1) + 1;
        velocity.y = -(NextEmitterRandom() & 1);
        velocity.z = 0;
        ActorParticles_CreateAtOffset(actor, &offset, &velocity, 0x4384, 0xf62b, 0, 0, 0, 0, 0xb8,
                                      30, 0, !(i & 1) ? 512 : 1024, 1, 0);
    }
}

struct State {
    u8 pad0[0x14];
    s32 value, limit;
};

s32 Div(s32, s32);

void Character_SetLowResourceColorEffect(u8 *p, struct State *s)
{
    s32 six = Div(s->limit * 6, 10);
    s32 four = Div(s->limit * 4, 10);
    s32 two = Div(s->limit * 2, 10);
    u32 result;
    if (s->value <= two) {
        p[0] = 20;
        p[2] = 2;
        p[3] = 1;
        return;
    } else if (s->value <= four) {
        p[0] = 40;
        p[2] = 2;
        p[3] = 1;
        return;
    } else if (s->value <= six) {
        p[0] = 60;
        p[2] = 1;
        p[3] = 1;
        return;
    } else
        p[3] = 0;
}

/* tentative: counts a held charge, spawning particles, and fires the callback on release */

struct Pos {
    s16 x, y, z, w;
};

struct Size {
    s16 w, h, a, b;
};

struct Input {
    u16 held;
    u16 pad;
    u16 pressed;
};

struct Owner {
    u8 filler0[0x20];
    struct Pos pos;
    u8 filler28[0x251 - 0x28];
    s8 markerSlot;
};

struct Self {
    u8 filler0[0x58];
    s16 value;
    u8 filler5A[0x2B0 - 0x5A];
    u32 callback;
    u8 filler2B4[0x2B8 - 0x2B4];
    u16 threshold;
    u8 filler2BA[0x2BE - 0x2BA];
    u16 timer;
};

void Sound_PlaySong(u32);

s32 Mod(s32, s32);

void ActorMarker_ShowSlot(s32);

void ActorMarker_HideSlot(s32);


s32 SpriteParticle_SpawnSingleFrameBatch(struct Pos *, struct Pos *, struct Size *, u32, u32, u32,
                                         u32);

void ChargeMarker_TickHold(struct Self *self, struct Owner *owner, struct Input *input)
{
    u16 held;

    if ((held = input->held & 2) != 0) {
        if (self->value >= self->threshold) {
            u16 t = ++self->timer;
            if (t == 10)
                Sound_PlaySong(0x1F8);
            else if (t == 50)
                Sound_PlaySong(0x1F8);
            if (self->timer > 60) {
                ActorMarker_ShowSlot(owner->markerSlot);
            } else {
                s32 r = Mod(self->timer, 12);
                if (r == 0) {
                    struct Pos pos;
                    struct Size size;
                    u32 n;
                    pos = owner->pos;
                    pos.x += 32;
                    pos.y -= 16;
                    size.w = 0x16;
                    size.h = 0x14;
                    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3FF;
                    n = gUnk_086141BC[gEffectRandomTableIndex];
                    n &= 3;
                    n++;
                    SpriteParticle_SpawnSingleFrameBatch(&pos, &owner->pos, &size, n, 1, r, r);
                }
            }
        } else {
            self->timer = 0;
            ActorMarker_HideSlot(owner->markerSlot);
        }
    } else if ((input->pressed & 2) && self->timer) {
        sub_08249240(&self->callback, 0, self->callback);
        self->timer = held;
        ActorMarker_HideSlot(owner->markerSlot);
    }
}
