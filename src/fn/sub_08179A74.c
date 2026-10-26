#define gMeterOwnerSource gUnk_020004B4
#include "global.h"
#include "engine.h"
struct MeterDisplayWidget {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0xE];
    u8 attribute;
    u8 unknown1B[5];
    s16 x, y, z;
    u8 unknown26[0x22];
    void *palette;
    u8 unknown4C[0x14];
};
struct MeterDisplay {
    u8 unknown0[0x5C];
    struct MeterDisplayWidget widgets[2];
    u8 resource[0x20];
    void *script;
    s32 scale;
};
struct MeterOwnerSource {
    u8 unknown0[0x18];
    s16 index;
};
extern struct MeterOwnerSource *gMeterOwnerSource;
extern u32 gUnk_030053F0;
s32 ViewportGauge_ComputeFill(struct MeterDisplay *, s32);
static inline s32 OwnerIndex(void)
{
    if (!gMeterOwnerSource)
        return -1;
    return gMeterOwnerSource->index;
}
void MeterDisplay_UpdateOwnerMeters(struct MeterDisplay *s)
{
    u32 mask = 0x800;
    if ((gEngineFlags | gUnk_030053F0) & mask) {
        s->widgets[0].x = ViewportGauge_ComputeFill(s, OwnerIndex());
        s->widgets[1].x = ViewportGauge_ComputeFill(s, OwnerIndex() ^ 1);
    } else {
        s->widgets[0].x = ViewportGauge_ComputeFill(s, 0);
        s->widgets[1].flags |= 1;
    }
}
