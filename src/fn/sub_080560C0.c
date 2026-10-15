#include "terrain_edge_marker.h"
#define TerrainEdgeMarker_Add sub_080560C0
void sub_08217DE0(struct TerrainMarkerSprite *, void *, u32);
void sub_08217EEC(struct TerrainMarkerSprite *, void *, u32);
void sub_08217EC4(struct TerrainMarkerSprite *, s32, s32);
void TerrainEdgeMarker_Add(union TerrainMarkerPosition *position, s32 direction, s32 kind,
                           s32 preserveHeight)
{
    struct TerrainMarkerManager *manager = sub_0821A2C4(0xFD22);
    if (manager) {
        void *resource = AnimData_FindEntry(0x1C1A);
        struct TerrainMarkerSprite *marker = &manager->markers[manager->count];
        sub_08217DE0(marker, resource, 0);
        if (kind == 0)
            sub_08217EEC(marker, resource, 0x33);
        else
            sub_08217EEC(marker, resource, kind + 0x34);
        sub_08217EC4(marker, -8, -8);
        marker->position = *position;
        if (!preserveHeight) {
            s32 x = position->world.x >> 8;
            s32 z = position->world.z >> 8;
            u8 *cell;
            if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
                x = 0;
            else {
                s32 addend = x;
                u32 row = gUnk_030052F4->rows[z];
                x = row + addend;
            }
            {
                u8 *cells = gUnk_030052F4->cells;
                if (!cells)
                    cell = 0;
                else {
                    u32 offset = x * 4 + 12;
                    cell = cells + offset;
                }
            }
            marker->position.world.y = (*cell & 15) * 256 + 128;
        }
        marker->renderMode = 1;
        if (direction) {
            if (direction == 1)
                marker->flags |= 4;
            else if (direction == 2)
                marker->flags |= 12;
            else
                marker->flags |= 8;
        }
        manager->count++;
    }
}
