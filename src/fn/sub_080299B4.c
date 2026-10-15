#include "global.h"
struct Packed {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void Collider_Init(void *, int, int, int, int, void *, void *);
void Collider_SetPosition(void *, void *, int);
void Collider_SetCallback(void *, void *, void *);
void sub_0821FF58(void *, int, int, int, int, int);
void sub_0821FF7C(void *, int, int, int);
s32 Collider_Refresh(void *);
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
    Collider_Init(collider, 0, 0x5101, 0, 16, &size, &offset);
    Collider_SetPosition(collider, p + 0x8a4, 0);
    Collider_SetCallback(collider, sub_08029870, p);
    sub_0821FF7C(collider, mode, 0x808, 4);
    Collider_Refresh(collider);
}
