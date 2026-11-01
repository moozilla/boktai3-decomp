#include "terrain_probe.h"

u8 sub_0821C254(u8 *, u16 *, struct TerrainProbe *, u8);
void Terrain_SampleSurface(u8 *, u16 *, struct TerrainProbe *);

void sub_0821C884(u8 *out, u16 *pos, u16 *delta, u32 unused, u32 unused2, u8 flag)
{
    struct TerrainProbe probe;
    probe.x = pos[0] + delta[0];
    probe.y = pos[1] + delta[1];
    probe.z = pos[2] + delta[2];
    probe.w = 0x10;

    *out = sub_0821C254(out, pos, &probe, flag);
    Terrain_SampleSurface(out, pos, &probe);
}
