#define EffectQuad_UpdateEntries  sub_08232E6C
#define gEffectQuadStateCallbacks gUnk_08602CA0
#include "global.h"
struct EffectQuadEntry {
    u8 state;
    u8 unknown1[0x37];
};
struct EffectQuad {
    u8 unknown0[0x20];
    struct EffectQuadEntry entries[4];
};
struct EffectQuadCallbacks {
    void *functions[3];
};
extern const struct EffectQuadCallbacks gUnk_08602CA0;
s32 sub_08249240(void *, void *, void *);
s32 sub_08232E6C(struct EffectQuad *quad)
{
    struct EffectQuadCallbacks callbacks = gUnk_08602CA0;
    struct EffectQuadEntry *entry = quad->entries;
    s32 i = 3;
    do {
        sub_08249240(quad, entry, callbacks.functions[entry->state]);
        i--;
        entry++;
    } while (i >= 0);
    return 0;
}
