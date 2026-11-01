#include "engine.h"
#include "contact_position.h"
#include "terrain_probe.h"

u32 sub_0821C254(u8 *, struct ContactPosition *, struct TerrainProbe *, u8);
void Terrain_SampleSurface(u8 *, struct ContactPosition *, struct TerrainProbe *);
u32 sub_0821BF5C(struct ContactPosition *, struct TerrainProbe *, u8);
s32 Mod(s32, s32);
s32 Div(s32, s32);

#define HIGH_EDGE(n)            ((((n) + 1) << 8) - 1)
#define LOW_EDGE(n)             (((n) << 8) + 1)
#define OFFSET(v, step, sign)   ((sign) > 0 ? (v) + (step) : (v) - (step))
#define UNOFFSET(v, step, sign) ((sign) > 0 ? (v) - (step) : (v) + (step))
#define BOUND(n, sign)          ((sign) > 0 ? HIGH_EDGE(n) : LOW_EDGE(n))

#define SET_FOOTPRINT_CORNER(xSign, zSign)                                                         \
    corner = destination;                                                                          \
    corner.x = OFFSET(corner.x, stepX, xSign);                                                     \
    corner.z = OFFSET(corner.z, stepZ, zSign);

#define SAMPLE_ADJUSTED_CORNER(xSign, zSign)                                                       \
    corner.x = UNOFFSET(corner.x, stepX, xSign);                                                   \
    corner.z = UNOFFSET(corner.z, stepZ, zSign);                                                   \
    Terrain_SampleSurface(track, pos, &corner);

#define TEST_FOOTPRINT_AXES(xSign, zSign)                                                          \
    probeX = *(struct TerrainProbe *)pos;                                                          \
    probeX.x += OFFSET(delta->x, stepX, xSign);                                                    \
    openX = sub_0821BF5C(pos, &probeX, flag);                                                      \
    probeZ = *(struct TerrainProbe *)pos;                                                          \
    probeZ.z += OFFSET(delta->z, stepZ, zSign);                                                    \
    openZ = sub_0821BF5C(pos, &probeZ, flag);

#define SAMPLE_BLOCKED_X(xSign)                                                                    \
    sub_0821C254(track, pos, &probeX, flag);                                                       \
    probeX.x = UNOFFSET(probeX.x, stepX, xSign);                                                   \
    probeX.z += delta->z;                                                                          \
    Terrain_SampleSurface(track, pos, &probeX);

#define SAMPLE_BLOCKED_Z(zSign)                                                                    \
    sub_0821C254(track, pos, &probeZ, flag);                                                       \
    probeZ.x += delta->x;                                                                          \
    probeZ.z = UNOFFSET(probeZ.z, stepZ, zSign);                                                   \
    Terrain_SampleSurface(track, pos, &probeZ);

#define PROBE_CARDINAL(x1, z1, x2, z2)                                                             \
    {                                                                                              \
        SET_FOOTPRINT_CORNER(x1, z1);                                                              \
        if (sub_0821C254(track, pos, &corner, flag)) {                                             \
            SAMPLE_ADJUSTED_CORNER(x1, z1);                                                        \
            track[0] = 1;                                                                          \
            return;                                                                                \
        } else {                                                                                   \
            SET_FOOTPRINT_CORNER(x2, z2);                                                          \
            if (sub_0821C254(track, pos, &corner, flag)) {                                         \
                SAMPLE_ADJUSTED_CORNER(x2, z2);                                                    \
                track[0] = 1;                                                                      \
                return;                                                                            \
            } else {                                                                               \
                Terrain_SampleSurface(track, pos, &destination);                                   \
            }                                                                                      \
        }                                                                                          \
    }

/* A blocked X axis finishes through the exit for its X direction. */
#define PROBE_DIAGONAL(xSign, zSign, finished, xBlocked)                                           \
    {                                                                                              \
        SET_FOOTPRINT_CORNER(xSign, zSign);                                                        \
        if (sub_0821C254(track, pos, &corner, flag)) {                                             \
            SAMPLE_ADJUSTED_CORNER(xSign, zSign);                                                  \
            track[0] = 1;                                                                          \
            return;                                                                                \
        } else {                                                                                   \
            struct TerrainProbe probeX, probeZ;                                                    \
            u32 openX, openZ;                                                                      \
            TEST_FOOTPRINT_AXES(xSign, zSign);                                                     \
            if (openX && openZ) {                                                                  \
                Terrain_SampleSurface(track, pos, &destination);                                   \
            } else if (!openX && !openZ) {                                                         \
                corner.x = BOUND(Mod(*(u16 *)(track + 6), gTerrainWidth), xSign);                  \
                corner.z = BOUND(Div(*(u16 *)(track + 6), gTerrainWidth), zSign);                  \
                SAMPLE_ADJUSTED_CORNER(xSign, zSign);                                              \
                goto finished;                                                                     \
            } else if (!openX) {                                                                   \
                SAMPLE_BLOCKED_X(xSign);                                                           \
                goto xBlocked;                                                                     \
            } else {                                                                               \
                SAMPLE_BLOCKED_Z(zSign);                                                           \
            finished:                                                                              \
                track[0] = 1;                                                                      \
                return;                                                                            \
            }                                                                                      \
        }                                                                                          \
    }

void Terrain_MoveFootprint(u8 *track, struct ContactPosition *pos, struct ContactPosition *delta,
                           u16 stepX, u16 stepZ, u8 flag)
{
    u32 mode = 0;
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
        case 0:
            Terrain_SampleSurface(track, pos, &destination);
            break;
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
            PROBE_DIAGONAL(1, 1, hitPosXPosZ, hitPosXNegZ);
            break;
        case 0x21:
            PROBE_DIAGONAL(-1, 1, hitNegXPosZ, hitNegXNegZ);
            break;
        case 0x12:
            PROBE_DIAGONAL(1, -1, hitPosXNegZ, hitPosXNegZ);
            break;
        case 0x22:
            PROBE_DIAGONAL(-1, -1, hitNegXNegZ, hitNegXNegZ);
            break;
    }
    return;
}
