#include "global.h"
struct Vec {s32 x,y,z,w;};
struct ShortVec {s16 x,y,z,w;};
struct P {
    u8 pad[0xc0];
    s32 boost;
    u8 pad2[0x4b8];
    struct ShortVec position;
    u8 pad3[8];
    struct ShortVec offset;
    u8 pad4[0x18];
    s32 speed;
    u8 pad5[0x1c];
    struct Vec velocity;
    u8 pad6[0x10];
    s8 decay;
};
struct Q {u8 pad[0x10];struct ShortVec position;u8 pad2[8];struct ShortVec offset;};
extern const s16 gUnk_086149C4[];
int sub_082215E4(int,int);
static inline int trunc12(int value)
{
    if(value>=0)return value>>12;
    return -((-value)>>12);
}
static inline void difference(struct Vec *a,struct Vec *b,struct Vec *out)
{
    out->x=a->x-b->x;
    out->z=a->z-b->z;
}
static inline int angle(struct Vec *v) {return sub_082215E4(v->x,v->z);}
void sub_0803CB40(struct P *p,struct Q *q)
{
    struct Vec target,origin,delta;
    struct ShortVec direction;
    int heading;
    origin.x=p->position.x+p->offset.x;
    origin.z=p->position.z+p->offset.z;
    target.x=q->position.x+q->offset.x;
    target.z=q->position.z+q->offset.z;
    difference(&origin,&target,&delta);
    heading=angle(&delta);
    heading=(heading>>5)<<5;
    direction.x=trunc12(gUnk_086149C4[(heading+64)&255]*8);
    direction.y=0;
    direction.z=trunc12(8*gUnk_086149C4[heading&255]);
    p->velocity.x=direction.x<<10;
    p->velocity.z=direction.z<<10;
    if((u32)(heading-10)>236) {
        p->speed+=0x3000;
        p->decay=10;
    } else {
        p->speed-=0x2000;
        p->boost=0;
        p->decay=15;
    }
}
