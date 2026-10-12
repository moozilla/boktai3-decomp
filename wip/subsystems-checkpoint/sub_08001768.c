#include "global.h"
#include "background_controller.h"
#define Background_Update sub_08001768
struct Camera { s16 x,y,unknown4,unknown6,offsetX,offsetY; u8 pad[6]; s16 mode; };
extern struct Camera gUnk_030042A0;
extern s16 gUnk_03005418[2];
extern u16 gUnk_03004BD8;
s32 sub_080015AC(struct Background *);
void sub_0821656C(u32,s32,s32,s32,s32);
void sub_08219574(u32,s32,s32);
static inline s32 truncate7(s32 v){if(v>=0)return v>>7;return -((-v)>>7);}
static inline s32 truncate3(s32 v){if(v>=0)return v>>3;return -((-v)>>3);}
s32 Background_Update(struct Background *p)
{
    s32 x,y,tileX,tileY;
    sub_080015AC(p);
    tileY=0;
    if(p->flags&2) {
        s32 cx,cy;
        if(!gUnk_030042A0.mode) {cx=gUnk_03005418[0]+gUnk_030042A0.offsetX;cy=gUnk_03005418[1]+gUnk_030042A0.offsetY;}
        else {cx=gUnk_030042A0.x>>3;cy=gUnk_030042A0.y>>3;}
        if(!(p->flags&1)){cx-=p->offsetX;cy-=p->offsetY;}
        if(p->flags&128){cx+=p->speedX;cy+=p->speedY;}
        if(p->rateX==128)x=cx;else x=truncate7(p->rateX*(cx-p->centerX))+p->centerX;
        x-=120;
        if(p->rateY==128)y=cy;else y=truncate7(p->rateY*(cy-p->centerY))+p->centerY;
        y-=80;
        tileX=truncate3(x-8);
        tileY=truncate3(y+80-128)&~1;
    } else {
        if(p->flags&4) {
            if(p->scrollDuration) {
                s32 remaining=p->scrollDuration-p->scrollTime;
                p->scrollX=Div(p->scrollStartX*remaining+p->scrollTargetX*p->scrollTime,p->scrollDuration)/128;
                p->scrollY=Div(p->scrollStartY*remaining+p->scrollTargetY*p->scrollTime,p->scrollDuration)/128;
                if(++p->scrollTime>p->scrollDuration) {
                    p->scrollDuration=0;p->scrollTime=0;
                    p->scrollX=p->scrollTargetX/128;p->scrollY=p->scrollTargetY/128;
                }
            }
            p->unknown38+=p->scrollX;p->unknown3C+=p->scrollY;
            p->x=p->unknown38+p->startX;p->y=p->unknown3C+p->startY;
        } else if(p->moveActive==1) {
            if(p->moveDuration) {
                s32 remaining=p->moveDuration-p->moveTime;
                p->x=Div(p->originX*remaining+p->targetX*p->moveTime,p->moveDuration);
                p->y=Div(p->originY*remaining+p->targetY*p->moveTime,p->moveDuration);
            }
            if(++p->moveTime>=p->moveDuration) {
                p->moveActive=0;p->moveTime=0;p->x=p->targetX;p->y=p->targetY;
            }
        }
        x=truncate7(p->x);y=truncate7(p->y);
        if(p->flags&32) {x+=gUnk_03005418[0]+gUnk_030042A0.offsetX;y+=gUnk_03005418[1]+gUnk_030042A0.offsetY;}
        if(x<0)tileX=~((-x)>>3);else tileX=x>>3;
        if(y<0)tileY=~((-y)>>3);else tileY=y>>3;
        tileY&=~1;
    }
    sub_0821656C(p->layer,tileX,tileY,x,y);
    if(p->fadeActive==1) {
        p->blendX=p->blendStepX+p->blendX;p->blendY=p->blendStepY+p->blendY;
        sub_08219574(p->layer,p->blendX>>8,p->blendY>>8);
        if(--p->fadeDuration==0)p->fadeActive=0;
    }
    if(p->flags&64) {
        if(p->frame&1)gUnk_03004BD8|=(0x01000000<<p->layer)>>16;
        else gUnk_03004BD8&=~((0x01000000<<p->layer)>>16);
    }
    p->frame++;
    return 0;
}
