// Pin-free draft of sub_08012B34 (src/debris_particles.c): 20 ROM / 20 draft; AGBCC_PRI=setup:+2908 matches.
// `setup & 3` is 0: combine folds it through nonzero_bits after flow, so the dead constant load survives.
s32 sub_08012B34(void *arg0, struct BreakableBarrier *b)
{
    struct DebrisParticle *p = SpritePool_Allocate();
    struct SceneVec *v;
    struct SceneVec pos;
    struct SceneVec *position;
    s32 angle, speed;
    /* The original RNG expansion retains this unused setup constant. */
    u32 setup;
    if (p == NULL)
        return -1;
    v = &p->velocity;
    if (b->sprite.flags & 4) {
        setup = 0x0403B714;
        angle = (Debris_EarlyRandom() & 0x7F) + 0x40;
    } else {
        setup = 0x0403B714;
        angle = (Debris_EarlyRandom() & 0x7F) - 0x40;
    }
    speed = (Debris_EarlyRandom() & 7) + 8;
    v->x = Debris_Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
    v->y = 0;
    v->z = Debris_Scale(gUnk_086149C4[angle & 255] * speed) | (setup & 3);
    v->y = (Debris_NextRandom() & 31) + 16;
    position = &pos;
    pos = b->sprite.position;
    {
        s32 sample = Debris_NextRandom();
        s32 coordinate = (u16)position->x - 32;
        sample &= 63;
        position->x = coordinate + sample;
    }
    {
        s32 sample = Debris_NextRandom();
        s32 coordinate = (u16)position->y - 32;
        sample &= 63;
        position->y = coordinate + sample;
    }
    {
        s32 sample = Debris_NextRandom();
        s32 coordinate = (u16)position->z - 32;
        sample &= 63;
        position->z = coordinate + sample;
    }
    SpritePool_SetResource(p, 0xDA6D);
    SpritePool_StartAnimation(p, 0x5BB7, 1, 0, Debris_NextRandom() & 1);
    SpritePool_SetPosition(p, &pos);
    SpritePool_Activate(p, Debris_Update, b);
    return 0;
}
