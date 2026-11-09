// Pin-free draft of sub_0801404C (ParticleGroup_UpdateBouncing): 38 ROM / 40 draft (no-pins start: 45 / 51).
void ParticleGroup_UpdateBouncing(struct Manager *m, struct Group *g)
{
    int i;
    int alive;
    u32 base = gUnk_08604B90[0x27];
    struct Particle *p;
    u8 *sprite;
    alive = 0;
    i = 0;
    if (i < g->count) {
        u16 *table = gRandomTable;
        p = g->particles;
        sprite = (u8 *)g + 20;
        do {
            if (p->active) {
                u16 index = CellIndex(&p->position);
                u8 *resource;
                int height;
                if (g->type <= 20)
                    resource = m->small;
                else
                    resource = m->large;
                p->position.x += p->velocity.x;
                p->position.z += p->velocity.z;
                p->height += p->velocity.y;
                p->velocity.y--;
                if (index != 0xffff) {
                    height = (Cell(index)->height & 15) << 8;
                    if (p->height < height) {
                        if (index != p->cell) {
                            int d = index - p->cell;
                            if (d < 0)
                                d = -d;
                            if (d == 1) {
                                u32 random = ParticleRandom_ReadSample(table);
                                p->velocity.x = -Scale(p->velocity.x, (random & 7) + 50);
                                if (index - p->cell < 0)
                                    p->position.x = (p->position.x & ~255) + 256;
                                else
                                    p->position.x = (p->position.x & ~255) - 1;
                            } else {
                                u32 random = ParticleRandom_ReadSample(table);
                                p->velocity.z = -Scale(p->velocity.z, (random & 7) + 50);
                                if (index - p->cell < 0)
                                    p->position.z = (p->position.z & ~255) + 256;
                                else
                                    p->position.z = (p->position.z & ~255) - 1;
                            }
                        } else
                            p->height = height;
                        {
                            u32 random = ParticleRandom_ReadSample(table);
                            p->velocity.y = -Scale(p->velocity.y, (random & 3) + 55);
                        }
                    }
                    {
                        u32 flags = Cell(index)->flags;
                        u32 mask = 32;
                        flags &= mask;
                        if (flags)
                            height = 0xffff;
                    }
                } else
                    height = 0xffff;
                p->cell = index;
                if ((m->timer & 1) == (i & 1) && height != 0xffff) {
                    p->position.y = height;
                    p->tile = *(u16 *)(resource + 4) + base;
                    p->priority = 3;
                } else {
                    p->position.y = p->height;
                    p->tile = *(u16 *)(resource + 4) + gUnk_08604B90[g->type];
                    p->priority = g->priority;
                }
                if (p->timer++ >= p->lifetime) {
                    p->active = 0;
                    Effect_Remove(sprite);
                } else
                    alive++;
            }
            p++;
            sprite += 56;
            i++;
        } while (i < g->count);
    }
    if (!alive)
        g->active = 0;
}
