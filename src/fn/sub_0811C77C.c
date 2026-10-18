#include "global.h"

struct S {
    u8 filler[0xd2];
    u16 h;
    u8 filler2[0x252 - 0xd4];
    s8 b;
};
s32 ActorResource_SelectElementResource(s32);

s32 CharacterModel_GetPrimaryPaletteIndex(struct S *s)
{
    s32 r;
    switch (s->h) {
        case 0:
            r = ActorResource_SelectElementResource(s->b);
            break;
        case 1:
            r = 0x106;
            break;
        case 2:
            r = 0x107;
            break;
        case 3:
            r = 0x108;
            break;
        case 4:
        case 5:
            r = 0x109;
            break;
        default:
            r = 0x378;
            break;
    }
    return r;
}
