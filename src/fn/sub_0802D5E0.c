#include "global.h"
struct Entry {
    u16 type;
    u8 variant, pad;
    void *data;
};
extern const struct Entry gUnk_086054D4[];
const struct Entry *sub_0802D5E0(int type, int variant)
{
    int i = 0;
    const struct Entry *p = gUnk_086054D4;
    for (; i < 32; p++, i++)
        if (p->type == type && p->variant == variant)
            return &gUnk_086054D4[i];
    return 0;
}
