#include "global.h"
#include "engine/engine.h"
/* Terrain_GetBoxMaxPosition: Copies a named box's upper corner into an output vector with vertical scaling */

u8 *Terrain_FindBoxesById(u16, void *);

void Terrain_GetBoxMaxPosition(u16 id, u16 *out)
{
    u32 tmp;
    u8 *e = Terrain_FindBoxesById(id, &tmp);
    if (e) {
        out[0] = *(u16 *)(e + 4);
        out[1] = e[9] << 4;
        out[2] = *(u16 *)(e + 6);
    }
}
