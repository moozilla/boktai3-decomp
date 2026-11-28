#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"
#include "scene/terrain/terrain_cell_probe.h"

#include "player/player.h"

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
                                      const struct ContactPosition *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

/* Play the impact cues and create a hazard at a nearby point on the same terrain level. */
void SceneActorAnimation_UpdateImpactHazard(struct StateActor *p, s32 tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)p;

    if (p->actionChanged) {
        s32 sequence;
        p->actionChanged = 0;
        sequence = (actor->graphics.animationFlags & 1) ? 26 : 64;
        SceneActors_StartDirectionalAnimation(p, &actor->animation, (int)&actor->animationResource,
                                              sequence, 2, 1, 4);
        Sound_PlaySongChecked(858);
    }
    if (p->actionTryUpdate(p))
        return;
    SceneActorState_SetFlags(p, 0x40);
    if (p->actionReset(p))
        return;
    if (tick == 16)
        Sound_PlaySongChecked(849);
    if ((tick & 15) == 15) {
        struct ContactPosition target = *((struct SceneGraphicsActor *)p)->playerPositions[0];
        s32 angle = (u8)((struct SceneGraphicsActor *)p)->players[0]->unk2D;
        const u16 *table = gRandomTable;
        u16 turn = Impact_Random(table) & 15;
        u16 reach = Impact_Random(table) & 127;
        struct Pos16 d;
        struct Pos16 *delta;
        s32 distance;
        delta = &d;
        angle = angle + turn;
        angle -= 8;
        distance = reach + 32;
        delta->x = s08025FFC_Trunc12(gUnk_086149C4[(angle + 0x40) & 0xff] * distance);
        delta->y = 0;
        angle &= 0xff;
        delta->z = s08025FFC_Trunc12(gUnk_086149C4[angle] * distance);
        Impact_AddPosition(&d, &d, &target);
        if (Impact_GetTerrainHeight(&d) == target.y)
            AnimatedHazard_Create(&d, actor->graphics.impactHazardResource, 60);
        else
            AnimatedHazard_Create(&target, actor->graphics.impactHazardResource, 60);
    }
    if (actor->animation.status & 2)
        p->actionFinished = 1;
}
