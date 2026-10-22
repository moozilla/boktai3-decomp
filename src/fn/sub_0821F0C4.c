#include "terrain_cell_probe.h"

// Path dispatcher: snap `out` onto a path point chosen by `mode`, falling back to a
// random point when the chosen helper finds none.

s32 Path_PickRandomPoint(struct Pos16 *out, u32 pathId);
s32 Path_SnapToNearestPoint(struct Pos16 *out, struct Pos16 *ref, u32 pathId, s32 distSq);
s32 Path_SnapToFarthestPoint(struct Pos16 *out, struct Pos16 *ref, u32 pathId, s32 distSq);
s32 Path_FindNearestDistantPoint(struct Pos16 *out, struct Pos16 *ref, u32 pathId, s32 distSq);
s32 Path_SnapToRandomFarPoint(struct Pos16 *out, struct Pos16 *ref, u32 pathId, s32 distSq);
s32 Path_SnapToRandomPointInBox(struct Pos16 *out, struct Pos16 *lo, struct Pos16 *hi, u32 pathId);

s32 sub_0821F0C4(struct Pos16 *out, struct Pos16 *ref, u32 mode, u32 pathId, s32 dist,
                 struct Pos16 *lo, struct Pos16 *hi)
{
    s32 d = dist;
    s32 result = -1;
    d *= d;
    switch (mode) {
        case 0:
            result = Path_PickRandomPoint(out, pathId);
            break;
        case 1:
            result = Path_SnapToNearestPoint(out, ref, pathId, d);
            break;
        case 2:
            result = Path_SnapToFarthestPoint(out, ref, pathId, d);
            break;
        case 3:
            result = Path_FindNearestDistantPoint(out, ref, pathId, d);
            break;
        case 4:
            result = Path_SnapToRandomFarPoint(out, ref, pathId, d);
            break;
        case 5:
            result = Path_SnapToRandomPointInBox(out, lo, hi, pathId);
            break;
    }
    if (result < 0)
        result = Path_PickRandomPoint(out, pathId);
    return result;
}
