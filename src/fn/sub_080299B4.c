#include "global.h"
struct Packed {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void sub_0821FEB4(void *, int, int, int, int, void *, void *);
void sub_0821FF24(void *, void *, int);
void sub_0821FF84(void *, void *, void *);
void sub_0821FF58(void *, int, int, int, int, int);
void sub_0821FF7C(void *, int, int, int);
void sub_0821FE40(void *);
void sub_08029904(void);
void sub_08029870(void *, void *, void *);
void sub_080299B4(u8 *p, int mode)
{
    u8 *collider = p + 0x818;
    struct Packed size, offset;
    size.x = 96;
    size.y = 128;
    size.z = 96;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    sub_0821FEB4(collider, 0, 0x5101, 0, 16, &size, &offset);
    sub_0821FF24(collider, p + 0x8a4, 0);
    sub_0821FF84(collider, sub_08029870, p);
    sub_0821FF7C(collider, mode, 0x808, 4);
    sub_0821FE40(collider);
}
