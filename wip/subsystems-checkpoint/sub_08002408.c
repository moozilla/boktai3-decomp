#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "affine.h"
#define AffineBackground_Update sub_08002408
struct Layer { u8 pad[32];s16 x,y;u8 tail[12]; };
struct Camera { s16 x,y,a,b,offsetX,offsetY; };
extern struct Layer gUnk_03004C30[];
extern struct Camera gUnk_030042A0;
extern s16 gUnk_03005418[2];
extern u16 gUnk_02000550,gUnk_0200053C;
extern const s16 gUnk_086149C4[];
s32 sub_080022A0(struct AffineBackground *);
void sub_08219574(u32,s32,s32);
static inline u32 fixedToInt(s32 v){if(v>=0)return v>>18;return -((-v)>>18);}
static inline s32 pixel(s16 v){if(v>=0)return v>>4;return -((-v)>>4);}
s32 AffineBackground_Update(struct AffineBackground *p)
{
    struct Layer *layer=&gUnk_03004C30[p->layer];
    s32 width,height;
    s32 cx,cy;
    sub_080022A0(p);
    if(!p->enabled)return 0;
    width=p->width<<4;height=p->height<<4;
    if(p->flags&8) {
        s32 speed=gUnk_02000550*(s16)p->speedScale;
        s32 angle=(gUnk_0200053C+160)&255;
        s32 x,y;
        const s16 *sine=gUnk_086149C4;
        p->accumulatedX+=sine[(angle+64)&255]*speed;
        p->accumulatedY+=sine[angle&255]*speed;
        x=fixedToInt(p->accumulatedX);y=fixedToInt(p->accumulatedY);
        if(x>width)x-=width;else if(x < -width)x+=width;
        if(y>height)y-=height;else if(y < -height)y+=height;
        p->x=p->centerX+x;p->y=p->centerY+y;
        layer->x=p->x;layer->y=p->y;
    } else {
        p->accumulatedX+=p->speedX;
        if(p->accumulatedX>width)p->accumulatedX-=width;else if(p->accumulatedX < -width)p->accumulatedX+=width;
        p->accumulatedY+=p->speedY;
        if(p->accumulatedY>height)p->accumulatedY-=height;else if(p->accumulatedY < -height)p->accumulatedY+=height;
        p->x=p->accumulatedX+p->centerX;p->y=p->accumulatedY+p->centerY;
        layer->x=pixel(p->x);layer->y=pixel(p->y);
    }
    cx=gUnk_03005418[0]+gUnk_030042A0.offsetX;
    cy=gUnk_03005418[1]+gUnk_030042A0.offsetY;
    if(p->width)layer->x+=Mod(cx-120,p->width);else layer->x=layer->x-120+cx;
    if(p->height)layer->y+=Mod(cy-80,p->height);else layer->y=layer->y-80+cy;
    if(p->fadeActive==1) {
        p->blendX=p->stepX+p->blendX;p->blendY=p->stepY+p->blendY;
        sub_08219574(p->layer,p->blendX>>8,p->blendY>>8);
        if(--p->fadeDuration==0)p->fadeActive=0;
    }
}
