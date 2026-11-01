#include "engine.h"
#include "contact_position.h"
#include "terrain_probe.h"

u32 sub_0821C254(u8 *, struct ContactPosition *, struct TerrainProbe *, u8);
void Terrain_SampleSurface(u8 *, struct ContactPosition *, struct TerrainProbe *);
u32 sub_0821BF5C(struct ContactPosition *, struct TerrainProbe *, u8);
s32 Mod(s32, s32);
s32 Div(s32, s32);

#ifdef NONMATCHING
// fuzzy: 82.2%
#define HIGH_EDGE(n)            ((((n) + 1) << 8) - 1)
#define LOW_EDGE(n)             (((n) << 8) + 1)
#define OFFSET(v, step, sign)   ((sign) > 0 ? (v) + (step) : (v) - (step))
#define UNOFFSET(v, step, sign) ((sign) > 0 ? (v) - (step) : (v) + (step))
#define BOUND(n, sign)          ((sign) > 0 ? HIGH_EDGE(n) : LOW_EDGE(n))

/* A successful corner probe may clamp its position. Undo the footprint offset on that result. */
#define PROBE_CARDINAL(x1, z1, x2, z2)                                                             \
    {                                                                                              \
        corner = destination;                                                                      \
        corner.x = OFFSET(corner.x, stepX, x1);                                                    \
        corner.z = OFFSET(corner.z, stepZ, z1);                                                    \
        if (sub_0821C254(track, pos, &corner, flag)) {                                             \
            corner.x = UNOFFSET(corner.x, stepX, x1);                                              \
            corner.z = UNOFFSET(corner.z, stepZ, z1);                                              \
            Terrain_SampleSurface(track, pos, &corner);                                            \
            goto adjusted;                                                                         \
        } else {                                                                                   \
            corner = destination;                                                                  \
            corner.x = OFFSET(corner.x, stepX, x2);                                                \
            corner.z = OFFSET(corner.z, stepZ, z2);                                                \
            if (sub_0821C254(track, pos, &corner, flag)) {                                         \
                corner.x = UNOFFSET(corner.x, stepX, x2);                                          \
                corner.z = UNOFFSET(corner.z, stepZ, z2);                                          \
                Terrain_SampleSurface(track, pos, &corner);                                        \
                goto adjusted;                                                                     \
            } else {                                                                               \
                Terrain_SampleSurface(track, pos, &destination);                                   \
            }                                                                                      \
        }                                                                                          \
    }

/* Probe each axis separately before clamping to a tile boundary. */
#define PROBE_DIAGONAL(xdir, zdir)                                                                 \
    {                                                                                              \
        corner = destination;                                                                      \
        corner.x = OFFSET(corner.x, stepX, xdir);                                                  \
        corner.z = OFFSET(corner.z, stepZ, zdir);                                                  \
        if (sub_0821C254(track, pos, &corner, flag)) {                                             \
            corner.x = UNOFFSET(corner.x, stepX, xdir);                                            \
            corner.z = UNOFFSET(corner.z, stepZ, zdir);                                            \
            Terrain_SampleSurface(track, pos, &corner);                                            \
            goto adjusted;                                                                         \
        } else {                                                                                   \
            struct TerrainProbe probeX, probeZ;                                                    \
            u32 openX, openZ;                                                                      \
            probeX = *(struct TerrainProbe *)pos;                                                  \
            probeX.x += OFFSET(delta->x, stepX, xdir);                                             \
            openX = sub_0821BF5C(pos, &probeX, flag);                                              \
            probeZ = *(struct TerrainProbe *)pos;                                                  \
            probeZ.z += OFFSET(delta->z, stepZ, zdir);                                             \
            openZ = sub_0821BF5C(pos, &probeZ, flag);                                              \
            if (openX && openZ) {                                                                  \
                Terrain_SampleSurface(track, pos, &destination);                                   \
            } else if (!openX && !openZ) {                                                         \
                corner.x = BOUND(Mod(*(u16 *)(track + 6), gTerrainWidth), xdir);                   \
                corner.z = BOUND(Div(*(u16 *)(track + 6), gTerrainWidth), zdir);                   \
                corner.x = UNOFFSET(corner.x, stepX, xdir);                                        \
                corner.z = UNOFFSET(corner.z, stepZ, zdir);                                        \
                Terrain_SampleSurface(track, pos, &corner);                                        \
                goto adjusted;                                                                     \
            } else if (!openX) {                                                                   \
                sub_0821C254(track, pos, &probeX, flag);                                           \
                probeX.x = UNOFFSET(probeX.x, stepX, xdir);                                        \
                probeX.z += delta->z;                                                              \
                Terrain_SampleSurface(track, pos, &probeX);                                        \
                goto adjusted;                                                                     \
            } else {                                                                               \
                sub_0821C254(track, pos, &probeZ, flag);                                           \
                probeZ.x += delta->x;                                                              \
                probeZ.z = UNOFFSET(probeZ.z, stepZ, zdir);                                        \
                Terrain_SampleSurface(track, pos, &probeZ);                                        \
                goto adjusted;                                                                     \
            }                                                                                      \
        }                                                                                          \
    }

void Terrain_MoveFootprint(u8 *track, struct ContactPosition *pos, struct ContactPosition *delta, u16 stepX,
                  u16 stepZ, u8 flag)
{
    s32 mode = 0;
    u8 clear;
    s32 tileX, tileZ, cell;
    struct TerrainProbe destination, corner;
    if (delta->x > 0)
        mode = 0x10;
    else if (delta->x < 0)
        mode = 0x20;
    if (delta->z > 0)
        mode |= 1;
    else if (delta->z < 0)
        mode |= 2;
    tileX = (s8)(pos->x >> 8);
    tileZ = (s8)(pos->z >> 8);
    if (tileX < 0 || tileZ < 0 || (u32)tileX >= gTerrainWidth || (u32)tileZ >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[tileZ] + tileX;
    clear = 0;
    *(u16 *)(track + 6) = cell;
    destination.x = pos->x + delta->x;
    destination.y = pos->y + delta->y;
    destination.z = pos->z + delta->z;
    destination.w = 0x10;
    track[0] = clear;
    switch (mode) {
        case 0x10:
            PROBE_CARDINAL(1, 1, 1, -1);
            break;
        case 0x20:
            PROBE_CARDINAL(-1, 1, -1, -1);
            break;
        case 0x01:
            PROBE_CARDINAL(1, 1, -1, 1);
            break;
        case 0x02:
            PROBE_CARDINAL(1, -1, -1, -1);
            break;
        case 0x11:
            PROBE_DIAGONAL(1, 1);
            break;
        case 0x21:
            PROBE_DIAGONAL(-1, 1);
            break;
        case 0x12:
            PROBE_DIAGONAL(1, -1);
            break;
        case 0x22:
            PROBE_DIAGONAL(-1, -1);
            break;
        case 0:
            Terrain_SampleSurface(track, pos, &destination);
            break;
    }
    return;
adjusted:
    track[0] = 1;
}
#else
INCLUDE_ASM("asm/nonmatching", Terrain_MoveFootprint);
#endif
