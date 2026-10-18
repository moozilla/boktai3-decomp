#include "global.h"
struct Entry {
    u16 type;
    u8 variant, pad;
    void *data;
};
extern const struct Entry gUnk_086053D4[];
void *Actor_FindResourceData(int type, int variant)
{
    int i = 0;
    const struct Entry *p = gUnk_086053D4;
    for (; i < 32; p++, i++)
        if (p->type == type && p->variant == variant)
            return p->data;
    return 0;
}
