#include "global.h"
struct P {u8 pad[0x10];s32 x,y,z;};
void sub_08116C4C(int,s32*);
int sub_0803C0D4(struct P *p)
{
    s32 position[4];
    int distance;
    sub_08116C4C(0,position);
    distance=(position[0]-p->x)>>12;
    if(distance<0)distance=-distance;
    if(distance<=100) {
        distance=(position[2]-p->z)>>12;
        if(distance<0)distance=-distance;
        if(distance<=30)return 1;
    }
    return 0;
}
