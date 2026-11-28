#include "engine/math/scalar_approach.h"
void sub_08221CC8(s16 *values, const s16 *centers, const s16 *ranges, s32 count)
{
    s16 *value = values;
    const s16 *center = centers;
    const s16 *range = ranges;

    while (--count >= 0) {
        s32 x = *value;
        s32 middle = *center;
        s32 radius = *range;
        s32 bound = middle - radius;
        s32 result;

        if (x < bound) {
            result = bound;
        } else {
            bound = middle + radius;
            result = x;
            if (result > bound)
                result = bound;
        }
        *value = result;
        value++;
        center++;
        range++;
    }
}
