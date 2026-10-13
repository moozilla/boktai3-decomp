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
void sub_08029908(u8 *p)
{
    u8 *collider = p + 0x7c4;
    struct Packed size, offset;
    size.x = 64;
    size.y = 128;
    size.z = 64;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    sub_0821FEB4(collider, 0, 0x2401, 0, 16, &size, &offset);
    sub_0821FF24(collider, p + 0x8a4, 0);
    sub_0821FF84(collider, sub_08029904, p);
    sub_0821FF58(collider, *(s16 *)(p + 0x1ac), *(s16 *)(p + 0x1ae), 0, 0, *(s16 *)(p + 0x1b0));
}
