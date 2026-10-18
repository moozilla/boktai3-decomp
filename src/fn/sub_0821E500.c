#include "global.h"
#include "engine.h"
/* Terrain_GetBoxMaxPosition: Copies a named box's upper corner into an output vector with vertical scaling */
#define Terrain_GetBoxMaxPosition sub_0821E500

u8 *Terrain_FindBoxesById(u16, void *);

void sub_0821E500(u16 id, u16 *out)
{
    u32 tmp;
    u8 *e = Terrain_FindBoxesById(id, &tmp);
    if (e) {
        out[0] = *(u16 *)(e + 4);
        out[1] = e[9] << 4;
        out[2] = *(u16 *)(e + 6);
    }
}
