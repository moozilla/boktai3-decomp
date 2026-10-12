#include "global.h"
#define ScrollingBackground_ApplyCommands sub_080022A0
struct ScrollingBackground {
    u8 task[0x1A];u8 flags,layer;
    s16 v1C,x,y,rotation,zoom,dx,dy;
    u8 pad2A[14];s16 targetX,targetY;
    u8 pad3C[8];u16 moveActive,moveTime,moveDuration,v4A;
    u8 enabled,pad4D;u16 fadeActive,fadeDuration,blendX,blendY,startX,startY,targetBlendX,targetBlendY,stepX,stepY,pad62;
    u32 count;u16 types[8];s16 args[8][8];
};
void sub_08219574(u32,s32,s32);
static inline s32 fixed4(s32 v) { return v << 4; }
s32 ScrollingBackground_ApplyCommands(struct ScrollingBackground *p)
{
    u32 i;
    for(i=0;i<p->count;i++) {
        u32 type=p->types[i];
        s16 *a=p->args[i];
        switch(type) {
        case 0:p->moveActive=0;p->moveTime=0;p->dx=0;p->dy=0;break;
        case 1:
            p->moveActive=1;p->moveDuration=a[0];p->moveTime=0;
            p->targetX=fixed4(a[1]);p->targetY=fixed4(a[2]);
            p->dx=Div(p->targetX-p->x,p->moveDuration);
            p->dy=Div(p->targetY-p->y,p->moveDuration);break;
        case 2:sub_08219574(p->layer,a[0],a[1]);break;
        case 3:
            p->fadeActive=1;p->startX=p->blendX;p->startY=p->blendY;
            p->fadeDuration=a[0];p->targetBlendX=a[1]<<8;p->targetBlendY=a[2]<<8;
            p->stepX=Div(p->targetBlendX-p->startX,p->fadeDuration);
            p->stepY=Div(p->targetBlendY-p->startY,p->fadeDuration);
            sub_08219574(p->layer,p->blendX>>8,p->blendY>>8);break;
        case 4:p->enabled=0;break;
        case 5:p->enabled=1;break;
        }
    }
    return 0;
}
