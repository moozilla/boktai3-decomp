#include "global.h"
// MATCHING_CONCESSION: register-pinning
/* Empty constraints preserve local register hints; they emit no instructions. */
#include "background_controller.h"

static inline s32 nextShift(void)
{
    u8 *p = Script_GetPc();
    return p ? sub_0821AA00(p) << 7 : 0;
}

s32 Background_LoadMapsFromScript(void)
{
    s32 indices[4];
    struct BackgroundResource resource;
    u8 *base = sub_08216E54(sub_0821ABA8('d', 0));
    s32 target, x, y, n, explicitIndices, i;
    resource = *(struct BackgroundResource *)base;
    resource.maps += (u32)base;
    resource.tiles += (u32)base;
    resource.extra += (u32)base;
    target = sub_0821ABA8('t', 1);
    if (Script_SeekToKeyword('o')) {
        u8 *cursor;
        x = ({
            s32 value = nextShift();
            asm("" : "+r"(value));
            value;
        });
        cursor = Script_GetPc();
        if (cursor)
            y = sub_0821AA00(cursor) << 7;
        else
            y = 0;
    } else {
        register s32 zero asm("r2") = 0;
        asm("" : "+r"(zero));
        x = zero;
        y = 0;
    }
    n = sub_0821ABA8('n', 1);
    explicitIndices = 0;
    if (Script_SeekToKeyword('i')) {
        u8 *p = Script_GetPc();
        explicitIndices = 1;
        for (i = 0; i < 4; i++) {
            if (p) {
                indices[i] = sub_0821AA00(p);
                p = Script_GetPc();
            } else
                indices[i] = (s32)p;
        }
    } else {
        s32 *out;
        i = 3;
        out = indices + 3;
        for (; i >= 0; i--) {
            *out = i;
            out--;
        }
    }
    if (!explicitIndices && Script_SeekToKeyword('c')) {
        u8 *p = Script_GetPc();
        s32 *out = indices;
        for (i = 3; i >= 0; i--) {
            if (p) {
                *out = Background_FindMapId(&resource, sub_0821AA00(p));
                p = Script_GetPc();
            }
            out++;
        }
    }
    if (base[0] == 'M' && base[1] == 'P') {
        register s32 drawX asm("r5") = x;
        asm("" : "+r"(drawX));
        sub_082161B4(target, 0, base, drawX, y, n, indices);
    }
}
