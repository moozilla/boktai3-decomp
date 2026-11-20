#include "scene_actors.h"
#include "scene_graphics_actor.h"
#include "terrain_cell_probe.h"

/* Play the impact cues and create a hazard at a nearby point on the same terrain level. */
union ImpactPosition {
    struct Pos16 vector;
    u32 words[2];
};

struct SceneActorImpactTarget {
    u8 unknown0[0x10C];
    u8 *owner; /* 0x10C: supplies the impact heading. */
    union ImpactPosition *position;
    u8 unknown114[0x36A - 0x114];
    s16 hazardResource;
};

static inline s32 s08025FFC_Trunc12(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}

static inline u32 Impact_GetTerrainHeight(struct Pos16 *pos)
{
    u8 *props = cellProps(cellIndexAt(pos));
    u32 type = *props >> 4;
    u32 height = (*props & 0xf) << 8;
    switch (type) {
        case 1:
            height -= (u8)pos->z;
            break;
        case 2:
            height -= (u8)pos->x;
            break;
    }
    return height;
}

static inline u16 Impact_Random(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}

static inline void Impact_AddPosition(struct Pos16 *out, const struct Pos16 *a,
                                      const struct Pos16 *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

void SceneActorAnimation_UpdateImpactHazard(struct StateActor *p, s32 arg)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;
    union ImpactPosition copy;
    struct Pos16 d;
    struct Pos16 *delta;
    u16 a;
    u16 b;
    s32 base;
    s32 nb;
    union ImpactPosition *src;
    s32 f;
    const u16 *table;

    if (p->actionChanged) {
        s32 seq;
        p->actionChanged = 0;
        seq = (actor->graphics.animationFlags & 1) ? 0x1a : 0x40;
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              seq, 2, 1, 4);
        Sound_PlaySongChecked(0x35A);
    }
    if ((u8)Callback_InvokeActor(p, (void *)p->actionTryUpdate))
        return;
    {
        u32 mask = 0x40;
        p->actionFlags |= mask;
    }
    if ((u8)Callback_InvokeActor(p, (void *)p->actionReset))
        return;
    if (arg == 0x10)
        Sound_PlaySongChecked(0x351);
    f = arg & 0xf;
    if (f == 0xf) {
        src = ((struct SceneActorImpactTarget *)p)->position;
        copy = *src;
        base = ((struct SceneActorImpactTarget *)p)->owner[0x2d];
        table = gRandomTable;
        a = Impact_Random(table) & f;
        b = Impact_Random(table) & 0x7f;
        delta = &d;
        base = base + a;
        base -= 8;
        nb = b + 0x20;
        delta->x = s08025FFC_Trunc12(gUnk_086149C4[(base + 0x40) & 0xff] * nb);
        delta->y = 0;
        base &= 0xff;
        delta->z = s08025FFC_Trunc12(gUnk_086149C4[base] * nb);
        Impact_AddPosition(&d, &d, &copy.vector);
        if (Impact_GetTerrainHeight(&d) == (s16)copy.vector.y) {
            AnimatedHazard_Create(&d, ((struct SceneActorImpactTarget *)actor)->hazardResource,
                                  0x3c);
        } else {
            AnimatedHazard_Create(&copy.vector,
                                  ((struct SceneActorImpactTarget *)actor)->hazardResource, 0x3c);
        }
    }
    if (actor->animation.status & 2)
        p->actionFinished = 1;
}
