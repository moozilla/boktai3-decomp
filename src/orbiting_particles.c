#include "orbiting_particles.h"

u32 OrbitingParticles_HasActiveOrb(struct OrbitingParticles *s)
{
    s32 i = 0;
    struct OrbitingParticle *e;
    if (i < s->count) {
        s32 n = s->count;
        e = s->orbs;
        do {
            if (e->state == 1)
                return 1;
            e++;
            i++;
        } while (i < n);
    }
    return 0;
}

void OrbitingParticles_SpawnOrb(struct OrbitingParticles *o, struct OrbitingParticle *b)
{
    if (b->timer == 0) {
        if (o->duration < 0 || o->time < o->duration) {
            b->state = 1;
            gRandomIndex = (gRandomIndex + 1) & 0x3ff;
            b->angle = *(const u16 *)((const u8 *)gUnk_0203B400 + gRandomIndex * 2);
            b->rise = 0;
            b->sprite.flags &= ~1;
        }
    } else {
        b->timer--;
    }
}

void OrbitingParticles_Advance(struct OrbitingParticles *m, struct OrbitingParticle *p)
{
    p->rise += m->speed;
    if (!(p->timer & 3))
        SpriteDrawable_SelectTileFrame(&p->sprite, m->resource, m->frame + ((p->timer >> 2) & 1));
    p->timer++;
    if (p->rise >= m->limit) {
        p->state = 0;
        p->sprite.flags |= 1;
        p->timer = ((8 - m->count) << 2) + 4;
    }
}

static inline int scale(int v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

// MATCHING_CONCESSION: register-pinning
int OrbitingParticles_Update(struct OrbitingParticles *m)
{
    int i;
    i = 0;
    if (i < m->count) {
        struct SceneVec *position = &m->orbs[0].sprite.position;
        struct OrbitingParticle *cursor = m->orbs;
        struct OrbitingParticle *p = cursor;
        do {
            int angle, radius;
            struct SceneVec *out;
            register const s16 *table asm("r2");
            struct OrbitingParticle *arg = cursor;
            asm("" : "+r"(arg));
            if (!p->state)
                OrbitingParticles_SpawnOrb(m, arg);
            else
                OrbitingParticles_Advance(m, arg);
            out = position;
            angle = p->angle;
            radius = p->radius;
            table = gUnk_086149C4;
            out->x = scale(table[(angle + 64) & 255] * radius);
            out->y = 0;
            out->z = scale(table[angle] * radius);
            p->sprite.position.y += p->rise;
            p->sprite.position.x += m->position.x;
            p->sprite.position.y += m->position.y;
            p->sprite.position.z += m->position.z;
            position = (void *)((u8 *)position + sizeof(struct OrbitingParticle));
            p++;
            cursor++;
            i++;
        } while (i < m->count);
    }
    {
        u32 *flags;
        i = m->count;
        if (i <= 7) {
            /* Keep the byte-offset temporary: direct indexing changes register allocation. */
            int mask = 1, offset;
            offset = i * sizeof(struct OrbitingParticle) + (u32) &
                     ((struct OrbitingParticles *)0)->orbs[0].sprite.flags;
            flags = (void *)(offset + (u32)m);
            do {
                *flags |= mask;
                flags = (void *)((u8 *)flags + sizeof(struct OrbitingParticle));
                i++;
            } while (i <= 7);
        }
    }
    m->time++;
    if (m->duration > 0 && m->time >= m->duration && !OrbitingParticles_HasActiveOrb(m))
        Task_Kill(m);
    return 0;
}

u32 OrbitingParticles_Destroy(struct OrbitingParticles *p)
{
    s32 i;
    struct OrbitingParticleSprite *q = &p->orbs[0].sprite;
    for (i = 7; i >= 0; i--) {
        Effect_Remove(q);
        /* Embedded sprites are spaced by a whole orb, not a sprite. */
        q = (void *)((u8 *)q + sizeof(struct OrbitingParticle));
    }
    return 0;
}

static inline u8 *cell(int i)
{
    u8 *p = TerrainModifiers_FindCellOverride(i, 1);
    if (p)
        return p + 4;
    return gTerrainMap->cells + 12 + i * 4;
}

static inline int randomAngle(void)
{
    u32 offset, value;
    u16 *table;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    table = ((u16 *)RANDOM_TABLE_ADDR);
    value = *(u16 *)(offset + (u32)table);
    return (value & 127) - 32;
}

int OrbitingParticles_Initialize(struct OrbitingParticles *m, struct SceneVec *source, int radius,
                                 int duration, int count, int type)
{
    int i, index, height, slope;
    struct SceneVec *pos;
    u8 *c;
    struct OrbitingParticle *p;
    switch (type) {
        case 1:
            m->resource = AnimData_FindEntry(0x1c1e);
            m->frame = 0;
            break;
        case 3:
            m->resource = AnimData_FindEntry(0x1c1e);
            m->frame = 4;
            break;
        case 4:
            m->resource = AnimData_FindEntry(0x1c1e);
            m->frame = 6;
            break;
        case 5:
            m->resource = AnimData_FindEntry(0x1c1e);
            m->frame = 8;
            break;
        case 6:
            m->resource = AnimData_FindEntry(0x1c1e);
            m->frame = 10;
            break;
        case 7:
            m->resource = AnimData_FindEntry(0x1c1e);
            m->frame = 12;
            break;
        case 8:
            m->resource = AnimData_FindEntry(0x1c1e);
            m->frame = 14;
            break;
        case 0:
        case 2:
        default:
            m->resource = AnimData_FindEntry(0x1c1e);
            m->frame = 2;
            break;
    }
    m->position = *source;
    pos = &m->position;
    {
        int x = pos->x >> 8, z = pos->z >> 8;
        if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
            index = 0;
        else
            index = gTerrainMap->rows[z] + x;
    }
    c = cell(index);
    slope = *c >> 4;
    height = (*c & 15) << 8;
    switch (slope) {
        case 1:
            height -= (u8)pos->z;
            break;
        case 2:
            height -= (u8)pos->x;
            break;
    }
    if (m->position.y < height)
        m->position.y = height;
    m->time = 0;
    m->duration = duration;
    m->speed = 20;
    m->limit = 400;
    if (count > 7)
        count = 8;
    m->count = count;
    p = m->orbs;
    for (i = 0; i <= 7; p++) {
        struct OrbitingParticleSprite *s = &p->sprite;
        int angle, r;
        p->state = 0;
        p->angle = randomAngle();
        p->rise = 0;
        p->radius = radius;
        p->timer = i * 4;
        ParticleSprite_Initialize(s, m->resource, 1);
        Effect_SetOrigin(s, -4, -4);
        SpriteDrawable_SelectTileFrame(s, m->resource, m->frame);
        Effect_SetPalette(s, 1);
        angle = p->angle;
        r = p->radius;
        p->sprite.position.x = scale(gUnk_086149C4[(angle + 64) & 255] * r);
        p->sprite.position.y = 0;
        p->sprite.position.z = scale(gUnk_086149C4[angle] * r);
        p->sprite.position.y += p->rise;
        p->sprite.position.x += m->position.x;
        p->sprite.position.y += m->position.y;
        p->sprite.position.z += m->position.z;
        {
            int one = 1;
            s->priority = one;
            i += one;
        }
    }
    return 0;
}

struct OrbitingParticles *OrbitingParticles_Create(struct SceneVec *pos, int radius, int duration,
                                                   int count, int type)
{
    struct OrbitingParticles *p = Task_Create(11, 436);
    if (p) {
        Task_SetCallbacks(p, (void (*)(void *))OrbitingParticles_Update,
                          (void (*)(void *))OrbitingParticles_Destroy);
        if (OrbitingParticles_Initialize(p, pos, radius, duration, count, type) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

struct OrbitingParticles *OrbitingParticles_CreateFromScript(void)
{
    struct OrbitingParticles *p = Task_Create(11, 436);
    struct SceneVec pos;
    int radius, duration, count, type;
    if (p) {
        if (Script_SeekToKeyword('p')) {
            pos.x = Script_GetValue();
            pos.y = Script_GetValue();
            pos.z = Script_GetValue();
        } else {
            pos.x = 0;
            pos.y = 0;
            pos.z = 0;
        }
        radius = Script_GetKeywordValue('d', 256);
        duration = Script_GetKeywordValue('t', 256);
        count = Script_GetKeywordValue('n', 8);
        type = Script_GetKeywordValue('T', 0);
        Task_SetCallbacks(p, (void (*)(void *))OrbitingParticles_Update,
                          (void (*)(void *))OrbitingParticles_Destroy);
        if (OrbitingParticles_Initialize(p, &pos, radius, duration, count, type) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

void OrbitingParticles_Stop(struct OrbitingParticles *p)
{
    if (p) {
        p->duration = p->time;
    }
}
