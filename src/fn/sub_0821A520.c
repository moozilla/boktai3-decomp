#include "global.h"
struct E { u32 k, v; };
extern struct E gUnk_08614D6C[];
u8 *sub_0821A490(u8 *, u32, u16, u16);
u32 sub_0821A4E8(u32, struct E *, s32, s32);
u8 *sub_0821A520(u16 key, u16 context)
{
    u32 special = 0;
    u16 saved_context = context;
    u32 combined;
    u8 *result;
    switch (key) {
    case 0x922E: key = 0x9225; context = 0x5130; special = 1; break;
    case 0x92B3: key = 0x9305; context = 0xD710; special = 1; break;
    case 0x9B1B: key = 0x9A65; context = 0x4679; special = 1; break;
    case 0x98F5: key = 0x9B05; context = 0x2117; special = 1; break;
    case 0xA635: key = 0xA705; context = 0x6D24; special = 1; break;
    case 0xAE6C: key = 0xAF05; context = 0xAC2C; special = 1; break;
    case 0xC091: key = 0xC305; context = 0xE53E; special = 1; break;
    case 0xCB05: key = 0xC8E5; context = 0x5F29; special = 1; break;
    case 0xCEEF: key = 0xCEE5; context = 0x4F2D; special = 1; break;
    case 0xCEAA: key = 0xCF05; context = 0xA4D; special = 1; break;
    }
    combined = (key << 16) | context;
    result = (u8 *)sub_0821A4E8(combined, gUnk_08614D6C, 0, 10);
    if (special == 1) {
        u32 third = saved_context;
        result = sub_0821A490(result, key, third, combined);
    }
    return result;
}
