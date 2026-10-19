#include "global.h"
#ifdef NONMATCHING
// fuzzy: 76.2%
// Instruction selection and order match the ROM; only register allocation differs. The
// ROM keeps the emitter in r6, the random index in r1, the table in r4 and the seed pointer
// in r5 (rasolve: raise the seed pointer's priority). Pinning cascades into high registers.
#include "engine.h"

struct Unk0823614CPosition {
    s16 x, y, z;
};
struct Unk0823614CParticle {
    u32 flags;
    u8 unknown4[0xB];
    u8 visible;
    u8 unknown10[8];
    s16 x, y, z;
    u8 unknown1E[0xA];
    u8 active;
    u8 unknown29;
    u8 angle;
    u8 tile;
    u16 timer;
    u8 unknown2E[4];
    u16 radius;
    struct Unk0823614CPosition *origin;
    u8 unknown38[8];
    void *update;
};
struct Unk0823614CEmitter {
    u8 next, delay, spawned, unknown3;
    void *graphics;
    struct Unk0823614CParticle particles[12];
};
void SpriteDrawable_SelectTileFrame(void *, void *, u32);
void sub_08236024(void);

static inline s32 Scale12(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

void sub_0823614C(u8 *owner, struct Unk0823614CPosition *origin)
{
    struct Unk0823614CEmitter *emitter = (struct Unk0823614CEmitter *)(owner + 0x138);

    if (++emitter->delay > 2) {
        struct Unk0823614CParticle *p = &emitter->particles[emitter->next];
        const u16 *table;
        u32 *seed;
        u32 i, mask;

        p->active = 1;
        p->origin = origin;
        table = gUnk_0203B400;
        seed = &gUnk_03005308;
        i = *seed + 1;
        mask = 1023;
        i &= mask;
        {
            u32 r = table[i];
            p->radius = (r & 0x7F) + 0x180;
        }
        i = (i + 1) & mask;
        p->angle = table[i];
        i = (i + 1) & mask;
        *seed = i;
        {
            u32 r = table[i];
            p->tile = (r & 1) + 2;
        }
        p->timer = 0;
        p->update = sub_08236024;
        p->flags &= ~1;
        SpriteDrawable_SelectTileFrame(p, *(void **)(owner + 0x13C), 4);
        p->x = p->origin->x + Scale12(p->radius * gUnk_086149C4[(p->angle + 0x40) & 0xFF]);
        p->y = p->origin->y;
        p->z = p->origin->z + Scale12(p->radius * gUnk_086149C4[p->angle]);
        p->visible = 1;
        if (++emitter->next > 11)
            emitter->next = 0;
        emitter->delay = 0;
        owner[0x13A] = 1;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0823614C);
#endif
