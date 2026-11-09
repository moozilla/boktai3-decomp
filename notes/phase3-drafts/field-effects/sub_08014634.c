// Plain rewrite of sub_08014634 (ParticleGroup_SpawnBouncing, src/fn/sub_08014634.c): 132 ROM / 116 draft,
// but structurally aligned (the pinned source's pins all removed is 92 / 86 from a much less natural base).
static inline s32 Random(u16 *table, u32 mask, u32 *rng)
{
    u32 r = (*rng + 1) & mask;
    *rng = r;
    r *= 2;
    return *(u16 *)(r + (u32)table);
}
s32 ParticleGroup_SpawnBouncing(s32 count, s32 options, const struct ContactPosition *position,
                                const struct ContactPosition *velocity, const struct ContactPosition *spread,
                                s32 lifetime, s32 lifetimeMask)
{
    struct Manager *m;
    u8 *resource;
    struct Group *g;
    s32 priority;
    s32 i, j;
    u32 *rng;

    m = gParticleGroupManager;
    if (!m) {
        m = ParticleManager_Create(0, 0);
        if (!m)
            return -1;
    }
    priority = options >> 8;
    options &= 255;
    if (count > 8)
        count = 8;
    if (options <= 20)
        resource = m->small;
    else
        resource = m->large;
    rng = &gRandomIndex;
    g = m->groups;
    for (j = 0; j < 6; j++, g++) {
        if (!g->active) {
            g->active = 1;
            g->type = options;
            g->count = count;
            if (priority > 0)
                g->priority = priority - 1;
            else
                g->priority = 2;
            i = 0;
            if (i < count) {
                u16 *table = gRandomTable;
                struct Particle *p = g->particles;
                u8 *sprite = (u8 *)g + 20;
                u32 mask = 0x3ff;
                do {
                    p->active = 1;
                    p->pad = 1;
                    p->timer = 0;
                    p->lifetime = lifetime + (Random(table, mask, rng) & lifetimeMask);
                    p->cell = CellIndex((struct ContactPosition *)position);
                    p->height = position->y;
                    if (spread->x)
                        p->velocity.x = velocity->x + Mod(Random(table, mask, rng), spread->x);
                    else
                        p->velocity.x = velocity->x;
                    if (spread->y)
                        p->velocity.y = velocity->y + Mod(Random(table, mask, rng), spread->y);
                    else
                        p->velocity.y = velocity->y;
                    if (spread->z)
                        p->velocity.z = velocity->z + Mod(Random(table, mask, rng), spread->z);
                    else
                        p->velocity.z = velocity->z;
                    if ((Random(table, mask, rng) & 15) <= 7)
                        p->flags |= 4;
                    else
                        p->flags &= ~4;
                    p->flags &= ~1;
                    p->position = *position;
                    p->tile = *(u16 *)(resource + 4) + gUnk_08604B90[options];
                    p->priority = g->priority;
                    sub_08217E40(sprite, 0);
                    p++;
                    sprite += 56;
                    i++;
                } while (i < count);
            }
            for (; i < 8; i++) {
                g->particles[i].active = 0;
                g->particles[i].flags |= 1;
            }
            break;
        }
    }
    return 0;
}
