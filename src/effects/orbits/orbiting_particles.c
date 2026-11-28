#include "effects/orbits/orbiting_particles.h"
#include "engine/math/math_shift.h"
#include "engine/math/monster_particle_random.h"
#include "scene/terrain/terrain_sampling.h"

u32 OrbitingParticles_HasActiveOrb(struct OrbitingParticles *particles)
{
    s32 i;

    for (i = 0; i < particles->count; i++) {
        if (particles->orbs[i].state == 1)
            return 1;
    }
    return 0;
}

void OrbitingParticles_SpawnOrb(struct OrbitingParticles *particles, struct OrbitingParticle *orb)
{
    if (orb->timer == 0) {
        if (particles->duration < 0 || particles->time < particles->duration) {
            orb->state = 1;
            orb->angle = ParticleRandom_ReadDirectSample((const u16 *)RANDOM_TABLE_ADDR);
            orb->rise = 0;
            orb->sprite.flags &= ~1;
        }
    } else {
        orb->timer--;
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

int OrbitingParticles_Update(struct OrbitingParticles *m)
{
    int i;
    struct OrbitingParticle *orb;
    i = 0;
    if (i < m->count) {
        struct SceneVec *position = &m->orbs[0].sprite.position;
        struct OrbitingParticle *p = m->orbs;
        do {
            int angle, radius;
            struct SceneVec *out;
            orb = &m->orbs[i];
            if (!p->state)
                OrbitingParticles_SpawnOrb(m, orb);
            else
                OrbitingParticles_Advance(m, orb);
            out = position;
            angle = p->angle;
            radius = p->radius;
            out->x = scale(gUnk_086149C4[(angle + 64) & 255] * radius);
            out->y = 0;
            out->z = scale(gUnk_086149C4[angle] * radius);
            p->sprite.position.y += p->rise;
            p->sprite.position.x += m->position.x;
            p->sprite.position.y += m->position.y;
            p->sprite.position.z += m->position.z;
            position = (void *)((u8 *)position + sizeof(struct OrbitingParticle));
            p++;
            i++;
        } while (i < m->count);
    }
    for (i = m->count; i < 8; i++) {
        orb = &m->orbs[i];
        orb->sprite.flags |= 1;
    }
    m->time++;
    if (m->duration > 0 && m->time >= m->duration && !OrbitingParticles_HasActiveOrb(m))
        Task_Kill(m);
    return 0;
}

u32 OrbitingParticles_Destroy(struct OrbitingParticles *particles)
{
    s32 i;

    for (i = 0; i < 8; i++)
        Effect_Remove(&particles->orbs[i].sprite);
    return 0;
}

int OrbitingParticles_Initialize(struct OrbitingParticles *particles, struct SceneVec *source,
                                 int radius, int duration, int count, int type)
{
    s32 i, height;
    struct OrbitingParticle *orb;

    switch (type) {
        case 1:
            particles->resource = AnimData_FindEntry(0x1C1E);
            particles->frame = 0;
            break;
        case 3:
            particles->resource = AnimData_FindEntry(0x1C1E);
            particles->frame = 4;
            break;
        case 4:
            particles->resource = AnimData_FindEntry(0x1C1E);
            particles->frame = 6;
            break;
        case 5:
            particles->resource = AnimData_FindEntry(0x1C1E);
            particles->frame = 8;
            break;
        case 6:
            particles->resource = AnimData_FindEntry(0x1C1E);
            particles->frame = 10;
            break;
        case 7:
            particles->resource = AnimData_FindEntry(0x1C1E);
            particles->frame = 12;
            break;
        case 8:
            particles->resource = AnimData_FindEntry(0x1C1E);
            particles->frame = 14;
            break;
        case 0:
        case 2:
        default:
            particles->resource = AnimData_FindEntry(0x1C1E);
            particles->frame = 2;
            break;
    }
    particles->position = *source;
    height = Terrain_SampleHeight((struct ContactPosition *)&particles->position);
    if (particles->position.y < height)
        particles->position.y = height;
    particles->time = 0;
    particles->duration = duration;
    particles->speed = 20;
    particles->limit = 400;
    if (count > 7)
        count = 8;
    particles->count = count;
    orb = particles->orbs;
    for (i = 0; i < 8; i++, orb++) {
        struct OrbitingParticleSprite *sprite = &orb->sprite;
        s32 angle, orbRadius;

        orb->state = 0;
        orb->angle = (ParticleRandom_ReadDirectSample(gRandomTable) & 127) - 32;
        orb->rise = 0;
        orb->radius = radius;
        orb->timer = i * 4;
        ParticleSprite_Initialize(sprite, particles->resource, 1);
        Effect_SetOrigin(sprite, -4, -4);
        SpriteDrawable_SelectTileFrame(sprite, particles->resource, particles->frame);
        Effect_SetPalette(sprite, 1);
        angle = orb->angle;
        orbRadius = orb->radius;
        orb->sprite.position.x =
            Math_ShiftTowardZero(gUnk_086149C4[(angle + 64) & 255] * orbRadius, 12);
        orb->sprite.position.y = 0;
        orb->sprite.position.z = Math_ShiftTowardZero(gUnk_086149C4[angle] * orbRadius, 12);
        orb->sprite.position.y += orb->rise;
        orb->sprite.position.x += particles->position.x;
        orb->sprite.position.y += particles->position.y;
        orb->sprite.position.z += particles->position.z;
        sprite->priority = 1;
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
