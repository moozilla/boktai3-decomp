#include "scene_panel_rig.h"

static inline s32 scale12(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}

static inline s32 scale3(s32 v)
{
    return v >= 0 ? v >> 3 : -((-v) >> 3);
}

static inline s32 scale5(s32 v)
{
    return v >= 0 ? v >> 5 : -((-v) >> 5);
}

// Places the six secondary sprites on a line from the rig's origin to its position (each
// a fixed blend of the two, lifted by the rig's height), circling it with a radius that
// grows along the chain, then steps each sprite's sequence.
void ScenePanelRig_TickSecondarySprites(struct ScenePanelRig *rig, u32 palette)
{
    s32 i = 0;
    for (;;) {
        // Strength reduced by loop.c: a counter on the stack, set up after the hoisted 255.
        s32 amplitude = i * 4 + 24;
        struct SceneRigSprite *sprite = &rig->secondary[i];
        struct SceneRigSprite *output = sprite;
        struct ContactPackedVector base;
        // Each pair below is one variable reused for a second value, as the ROM keeps each
        // pair in one register (r2, r3, r4); separate variables reorder the allocation.
        union {
            s32 near;
            const s16 *table;
        } nearOrTable;
        union {
            s32 far;
            u16 *randomPhase;
        } farOrRandomPhase;
        union {
            s32 lift;
            u16 *phase;
        } liftOrPhase;
        s32 radius;

        nearOrTable.near = i + 2;
        farOrRandomPhase.far = 8 - nearOrTable.near;
        liftOrPhase.lift =
            scale5(3 * ((rig->position.y - rig->origin.y) * farOrRandomPhase.far));
        base.x = scale3(rig->position.x * nearOrTable.near + rig->origin.x * farOrRandomPhase.far) +
                 liftOrPhase.lift;
        base.y = scale3(rig->position.y * nearOrTable.near + rig->origin.y * farOrRandomPhase.far);
        base.z = scale3(rig->position.z * nearOrTable.near + rig->origin.z * farOrRandomPhase.far) +
                 liftOrPhase.lift;
        nearOrTable.table = gUnk_086149C4;
        farOrRandomPhase.randomPhase = &sprite->randomPhase;
        radius = scale12(
            nearOrTable.table[((*farOrRandomPhase.randomPhase & 255) + 64) & 255] * amplitude);
        {
            s32 horizontal = nearOrTable.table[(sprite->phase + 64) & 255] * radius;
            liftOrPhase.phase = &sprite->phase;
            output->position.x = base.x + scale12(horizontal);
        }
        output->position.y = base.y;
        output->position.z =
            base.z + scale12(nearOrTable.table[*liftOrPhase.phase & 255] * radius);
        *liftOrPhase.phase = (*liftOrPhase.phase + 3) & 255;
        *farOrRandomPhase.randomPhase += 5;
        ScenePanelRig_StepSprite(sprite);
        AnimData_SelectPaletteBank(sprite->graphics, palette);
        i++;
        if (i > 5)
            break;
    }
}
