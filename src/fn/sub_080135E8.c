#include "global.h"
struct Manager {
    u8 pad[24];
    u8 *resource;
};
extern struct Manager *gUnk_02000054;
void *sub_080135A8(int, int);
void Shadow_UpdateTerrain(void *);
void sub_08013454(void *);
void sub_08217DE0(void *, void *, int);
void sub_08217EC4(void *, int, int);
void sub_080133CC(void *, void *);
int Shadow_Initialize(u8 *p, void *source, int mode)
{
    u8 *sprite;
    int zero;
    if (!gUnk_02000054 && !sub_080135A8(0, 0))
        return -1;
    zero = 0;
    p[0] = zero;
    *(void **)(p + 8) = source;
    p[1] = zero;
    *(u16 *)(p + 2) = zero;
    *(u16 *)(p + 4) = zero;
    if (!mode)
        *(void (**)(void *))(p + 0x34) = Shadow_UpdateTerrain;
    else
        *(void (**)(void *))(p + 0x34) = sub_08013454;
    sprite = p + 12;
    sub_08217DE0(sprite, gUnk_02000054->resource, 512);
    sub_08217EC4(sprite, -8, -8);
    {
        u8 *resource = gUnk_02000054->resource;
        int width = p[0x16] >> 3, height = p[0x17] >> 3;
        *(u16 *)(p + 0x14) = width * height * 34 + *(u16 *)(resource + 4);
    }
    p[0x1c] = 252;
    p[0x1b] = 2;
    sub_080133CC(gUnk_02000054, p);
    return 0;
}
