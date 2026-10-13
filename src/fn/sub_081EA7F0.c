#include "global.h"
/* Initialize the controller's background layers and palette. */
extern u8 gUnk_03004FC0[];
void sub_082156B8(u32);
void sub_0821656C(u32, u32, u32, u32, u32);
void *sub_0821A520(u32, u32);
void sub_082161B4(u32, u32, void *, u32, u32, u32, u32 *);
void sub_08215A74(u32, u32);
void sub_081EA7F0(u8 *p)
{
    void *resource;
    u32 layer;
    sub_082156B8(0);
    sub_0821656C(0, 0, 0, 0, 0);
    sub_082156B8(2);
    sub_082156B8(3);
    resource = sub_0821A520(0xc091, 0xc5a7);
    *(void **)(p + 0x720) = resource;
    layer = 0;
    sub_082161B4(1, 16, resource, 0, 0, 1, &layer);
    sub_08215A74(2, 2);
    layer = 1;
    sub_082161B4(2, 0, resource, 0, 0, 1, &layer);
    sub_08215A74(3, 3);
    sub_0821656C(3, 0, 0, 0, 0);
    CpuSet((u8 *)sub_0821A520(0x92b3, 0x91d) + 20, gUnk_03004FC0, 256);
}
