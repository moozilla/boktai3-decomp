#include "global.h"
#include "scene_actor_terrain.h"
#include "scene_object.h"

static inline u32 SceneGrid_IsCellStop(u8 *cell)
{
    u32 flags = ((struct SceneCell *)cell)->flags;
    flags &= 0x80;
    return flags;
}

/* Directional height scan; stopping relies on terrain flag 0x80. */
s32 SceneGrid_ScanTerrainDirection(s32 start, s32 angle)
{
    s32 direction = ((angle + 32) >> 6) & 3;
    s32 height = *SceneActorTerrain_GetCell(start) & 15;
    s32 cell = start;
    s32 best = 0;
    s32 next;
    s32 nextHeight;
    switch (direction) {
        case 0:
            for (;;) {
                next = cell + 1;
                nextHeight = *SceneActorTerrain_GetCell(next) & 15;
                if (SceneGrid_IsCellStop(SceneActorTerrain_GetCell(next))) {
                    if (best)
                        return best;
                    return cell;
                }
                if (nextHeight != height) {
                    if (best)
                        return best;
                    best = cell;
                } else {
                    best = next;
                }
                cell++;
            }
        case 1:
            for (;;) {
                next = cell + SceneActorTerrain_Width();
                nextHeight = *SceneActorTerrain_GetCell(next) & 15;
                if (SceneGrid_IsCellStop(SceneActorTerrain_GetCell(next))) {
                    if (best)
                        return best;
                    return cell;
                }
                if (nextHeight != height) {
                    if (best)
                        return best;
                    best = cell;
                } else {
                    best = next;
                }
                cell += SceneActorTerrain_Width();
            }
        case 2:
            for (;;) {
                next = cell - 1;
                nextHeight = *SceneActorTerrain_GetCell(next) & 15;
                if (SceneGrid_IsCellStop(SceneActorTerrain_GetCell(next))) {
                    if (best)
                        return best;
                    return cell;
                }
                if (nextHeight != height) {
                    if (best)
                        return best;
                    best = cell;
                } else {
                    best = next;
                }
                cell--;
            }
        case 3:
            for (;;) {
                next = cell - SceneActorTerrain_Width();
                nextHeight = *SceneActorTerrain_GetCell(next) & 15;
                if (SceneGrid_IsCellStop(SceneActorTerrain_GetCell(next))) {
                    if (best)
                        return best;
                    return cell;
                }
                if (nextHeight != height) {
                    if (best)
                        return best;
                    best = cell;
                } else {
                    best = next;
                }
                cell -= SceneActorTerrain_Width();
            }
    }
    return start;
}
