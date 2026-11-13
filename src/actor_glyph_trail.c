#include "actor_glyph_trail.h"
#include "scene_facing.h"
#include "engine.h"

void TimedSprite_UpdateMotion(struct ActorGlyphTrail *);
void ActorGlyphTrail_OnPhase(struct ActorGlyphTrail *);

void ActorGlyphTrail_SetState(struct ActorGlyphTrail *s, u32 v)
{
    s->phase = v;
    s->timer = 0;
}

extern const s16 gUnk_086149C4[];

static inline s32 Truncate(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

static inline s32 Primary(s32 value)
{
    s32 result = Truncate(value);
    return result;
}

// confidence=low: offsets a trail anchor to either side of its owner and sets its velocity.

void ActorGlyphTrail_UpdateAnchor(struct ActorGlyphTrail *p)
{
    struct GlyphOwner *owner = p->owner;
    s32 angle, cosine, sine;
    p->position = owner->position;
    p->position.y += 200;
    angle = ((owner->direction + 5) & 7) << 5;
    cosine = gUnk_086149C4[(angle + 64) & 255];
    sine = gUnk_086149C4[angle & 255];
    if (owner->side) {
        p->position.x += Primary(cosine * 170) + Truncate(sine * 8);
        p->position.z += Primary(sine * 170) - Truncate(cosine * 8);
    } else {
        p->position.x += Primary(cosine * 170) - Truncate(sine * 8);
        p->position.z += Primary(sine * 170) + Truncate(cosine * 8);
    }
    p->direction = SceneFacing_EightWay(angle);
    p->velocityX = Truncate(gUnk_086149C4[(angle + 64) & 255] * p->speed);
    p->velocityZ = Truncate(gUnk_086149C4[angle] * p->speed);
}

struct GlyphParticleView {
    struct GlyphPosition position;
    u8 pad8[8];
    u8 active;
    s8 angle;
    u16 radius, timer;
};

static inline s32 WidenAngle(s32 offset)
{
    offset += 256;
    return offset;
}

// confidence=low: advances four orbiting glyphs and hides each after fourteen frames.

void ActorGlyphTrail_UpdateParticles(struct ActorGlyphTrail *p)
{
    s32 direction = ((p->direction + 5) & 7) << 5;
    u32 mask = 255;
    const s16 *sines = gUnk_086149C4;
    s32 offset = 0;
    struct GlyphParticleView *slot = (void *)&p->particles[0].position;
    u16 *timer = &p->particles[0].timer;
    s32 count = 3;
    do {
        if (slot->active) {
            slot->timer++;
            if (slot->timer > 14) {
                slot->active = 0;
                {
                    struct GlyphTrailParticle *base = p->particles;
                    u32 *flags = (u32 *)(offset + (u8 *)base);
                    *flags |= 1;
                }
            } else {
                s32 radius = (slot->radius * (16 - (*timer))) >> 3;
                s32 angle = (direction + WidenAngle(slot->angle)) & mask;
                slot->position = p->position;
                slot->position.x += Truncate(sines[(angle + 64) & mask] * radius);
                slot->position.z += Truncate(sines[angle & mask] * radius);
            }
        }
        offset += sizeof(struct GlyphTrailParticle);
        slot = (void *)((u8 *)slot + sizeof(struct GlyphTrailParticle));
        timer = (void *)((u8 *)timer + sizeof(struct GlyphTrailParticle));
    } while (--count >= 0);
}

s32 Mod(s32, s32);

static inline s32 RandomValue(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}

// confidence=low: activates the next glyph with a random angular offset and radius.

void ActorGlyphTrail_EmitParticle(struct ActorGlyphTrail *p)
{
    s32 angle;
    const u16 *table;
    p->particles[p->particleIndex].active = 1;
    p->particles[p->particleIndex].flags &= ~1;
    p->particles[p->particleIndex].timer = 0;
    table = gRandomTable;
    p->particles[p->particleIndex].radius = ((RandomValue(table) >> 3) & 127) + 64;
    p->particles[p->particleIndex].angle = Mod(RandomValue(table), 96) - 48;
    angle = ((p->direction + 5) & 7) << 5;
    angle += WidenAngle(p->particles[p->particleIndex].angle);
    angle &= 255;
    p->particles[p->particleIndex].position = p->position;
    {
        s32 radiusX = p->particles[p->particleIndex].radius;
        p->particles[p->particleIndex].position.x += Truncate(gUnk_086149C4[(angle + 64) & 255] * radiusX);
    }
    {
        s32 radiusZ = p->particles[p->particleIndex].radius;
        p->particles[p->particleIndex].position.z += Truncate(gUnk_086149C4[angle] * radiusZ);
    }
    p->particleIndex++;
    if (p->particleIndex > 3)
        p->particleIndex = 0;
}

void ActorGlyphTrail_HideParticles(u8 *p)
{
    u8 z = 0;
    u32 one = 1;
    u8 *a = p + 0xA4;
    u8 *b = p;
    s32 i = 3;
    do {
        *a = z;
        *(u32 *)(b + 0x7C) |= one;
        a += 0x30;
        b += 0x30;
        i--;
    } while (i >= 0);
}

void ActorGlyphTrail_Update(struct ActorGlyphTrail *p)
{
    u32 state = p->owner[0x457];
    u8 *dp, *q;
    u16 *tp;
    if (state != 3) {
        Task_Kill(p);
        return;
    }
    ActorGlyphTrail_UpdateAnchor(p);
    if (p->owner[0x458] == 5) {
        ActorGlyphTrail_HideParticles(p);
        p->flags &= ~1;
        p->spriteFrame = p->state;
        ActorGlyphTrail_SetState(p, TimedSprite_UpdateMotion);
        return;
    }
    ActorGlyphTrail_UpdateParticles(p);
    if (p->owner[0x458] == 2) {
        u16 *tmp = &p->timer;
        state &= *tmp;
        tp = tmp;
        if (state == 3)
            ActorGlyphTrail_EmitParticle(p);
        q = &p->state;
        *q = 0;
        dp = q;
    } else {
        p->flags &= ~1;
        q = &p->state;
        *q = 1;
        dp = q;
        tp = &p->timer;
    }
    if ((*tp >> 2) & 1)
        p->spriteFrame = *dp + 1;
    else
        p->spriteFrame = *dp;
    (*tp)++;
}

#define TimedSprite_SetPhase ActorGlyphTrail_SetState

void TimedSprite_UpdateMotion(struct ActorGlyphTrail *p)
{
    p->position.x += p->velocityX;
    p->position.z += p->velocityZ;
    p->timer++;
    if (p->timer >= p->durationOrVariant)
        ActorGlyphTrail_SetState(p, ActorGlyphTrail_OnPhase);
}

struct ActorGlyphTrailObj {
    u32 flags;
    u8 pad[12];
    u16 frame;
};

void ActorGlyphTrail_OnPhase(struct ActorGlyphTrail *p)
{
    p->timer++;
    if (p->timer == 1) {
        p->spriteFrame = 4;
    } else if (p->timer == 4) {
        u8 *dp = &p->direction;
        struct ActorGlyphTrailObj *o = (struct ActorGlyphTrailObj *)((u8 *)p + 0x1c);
        s32 d = *dp;
        if (d & 1)
            o->frame = 7;
        else if ((d >> 1) & 1)
            o->frame = 8;
        else
            o->frame = 6;
        if (d <= 2)
            o->flags &= ~12;
        else if (d <= 4)
            o->flags = (o->flags & ~4) | 8;
        else if (d <= 5)
            o->flags |= 12;
        else
            o->flags = (o->flags | 4) & ~8;
    } else if (p->timer > 6) {
        Task_Kill(p);
    }
}

u32 ActorGlyphTrail_UpdateTask(u8 *s)
{
    Callback_InvokeActor(s, *(u32 *)(s + 0x13c));
    return 0;
}

struct E {
    u8 pad[0x30];
};

struct GlyphDestroyView {
    u8 pad[0x7c];
    struct E e[8];
};

s32 ActorGlyphTrail_DestroyTask(struct GlyphDestroyView *p)
{
    struct E *e;
    s32 i;
    Sprite_Remove((u8 *)p + 0x1C);
    e = p->e;
    for (i = 3; i >= 0; i--) {
        Effect_Remove(e);
        e++;
    }
    return 0;
}

void ActorGlyphTrail_InitSprite(u8 *s)
{
    u8 *p = s + 0x48;
    u8 *q;
    AnimData_Load(p, 0x210e);
    q = s + 0x1c;
    Sprite_Init(q, p, 1);
    *(u16 *)(q + 0x10) = 0;
    AnimData_SelectPaletteBank(p, 0x4d);
}

#define Effect_SetSpriteOffset Effect_SetOrigin

#define Effect_SetPaletteSlot  Effect_SetPalette

void *AnimData_FindEntry(s32);

void ParticleSprite_Initialize(void *, void *, u32);

void Effect_SetOrigin(void *, s32, s32);

void SpriteDrawable_SelectTileFrame(void *, void *, u32);

void Effect_SetPalette(void *, u32);

void SpriteBurst_InitParticles(struct ActorGlyphTrail *p)
{
    s32 i;
    struct GlyphTrailParticle *particle;
    u8 *active;
    s32 offset;
    p->particleIndex = 0;
    p->particleResource = AnimData_FindEntry(0x1c1e);
    offset = -4;
    active = &p->particles[0].active;
    particle = (void *)p->particles;
    for (i = 3; i >= 0; i--) {
        ParticleSprite_Initialize(particle, p->particleResource, 1);
        Effect_SetOrigin(particle, offset, offset);
        SpriteDrawable_SelectTileFrame(particle, p->particleResource, 0x11);
        Effect_SetPalette(particle, 1);
        *active = 0;
        active += 0x30;
        particle++;
    }
}

#define SpriteBurst_InitSprite  ActorGlyphTrail_InitSprite

#define SpriteBurst_SetCallback ActorGlyphTrail_SetState

s32 SpriteBurst_Init(struct ActorGlyphTrail *p, u32 parameter, u32 value, u32 variant)
{
    p->owner = parameter;
    p->speed = value;
    p->durationOrVariant = variant;
    p->state = 0;
    ActorGlyphTrail_InitSprite(p);
    ActorGlyphTrail_UpdateAnchor(p);
    SpriteBurst_InitParticles(p);
    ActorGlyphTrail_SetState(p, ActorGlyphTrail_Update);
    return 0;
}

void *ActorGlyphTrail_Create(s32 a, s32 b, s32 c)
{
    void *r = Task_Create(8, 0x140);
    if (r != 0) {
        Task_SetCallbacks(r, ActorGlyphTrail_UpdateTask, ActorGlyphTrail_DestroyTask);
        if (SpriteBurst_Init(r, a, b, c) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
