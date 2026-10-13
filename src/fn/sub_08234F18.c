#include "global.h"
struct Data {
    u32 words[8];
};
struct Data *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(struct Data *, struct Data *);
u16 sub_0822D604(u32);
void sub_0821983C(u8 *, struct Data *, u32, u32, u32, u32, u32, void *);
void sub_08234F18(u8 *p)
{
    struct Data data;
    struct Data *source = ScriptProc_Start(0xcb05, 0x5d04);
    u16 *coords;
    u32 zero;
    data = *source;
    ResourceHeader_Relocate(&data, source);
    coords = (u16 *)(p + 0x7c);
    zero = 0;
    coords[0] = 8;
    *(u16 *)(p + 0x7e) = 32;
    *(u16 *)(p + 0x80) = zero;
    sub_0821983C(p + 0x1c, &data, sub_0822D604(0x26), 0x33, zero, zero, 0x3c, coords);
}
