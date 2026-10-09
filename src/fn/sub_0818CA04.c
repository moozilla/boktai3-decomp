#include "global.h"
struct Obj {
    u32 flags;
    u8 pad4[0x2];
    u8 rotation;
    u8 pad7[0x1];
    s8 byte8;
    s8 byte9;
    u8 padA[0x12];
    s16 x;
    s16 y;
    s16 z;
    u8 pad22[0x2c];
    u16 flags4E;
    u8 pad50[0x52];
    u16 flagsA2;
    u8 padA4[0x4e];
    u16 velocity;
    u8 padF4[0x40];
    u16 endX;
    s16 endY;
    u16 endZ;
    u8 pad13A[0x2];
    u16 startX;
    u16 startY;
    u16 startZ;
    u8 pad142[0x2];
    s16 dx;
    s16 dy;
    s16 dz;
    u8 pad14A[0x12];
    u8 * parent;
    u8 pad160[0x70];
    void (*callback)(struct Obj *);
    u8 pad1D4[0x2];
    s16 timer;
    u8 pad1D8[0x5];
    s8 byte1DD;
    s8 delta8;
    s8 delta9;
};
void sub_0818CA6C(struct Obj *);
void sub_0818CA04(struct Obj *p)
{
    s32 a=p->byte8,b=p->byte9;
    if(a>64) { a+=p->delta8; if(a<64) a=64; }
    if(b<127) { b+=p->delta9; if(b>127) b=127; }
    p->byte8=a; p->byte9=b;
    if(b==127) { p->byte8=64; p->delta9=-16; p->callback=sub_0818CA6C; }
}
