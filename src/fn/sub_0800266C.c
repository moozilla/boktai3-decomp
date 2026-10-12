#include "global.h"
#include "affine_background.h"
#define AffineBackground_Initialize sub_0800266C
struct Config {u8 flags,layer;u16 resource;s32 x,y,offsetX,offsetY,speedX,speedY,scale,width,height,mode,priority,fade,blendX,blendY;};
struct Layer {u16 flags;u8 pad[46];};
struct Camera {s16 x,y,a,b,offsetX,offsetY;};
struct Dimensions {s16 pad[4],x,y;};
struct ResourceRef {u32 a;struct Dimensions *data;};
extern struct AffineBackground *gUnk_02000010[];
extern struct Layer gUnk_03004C30[];
extern struct Camera gUnk_030042A0;
extern s16 gUnk_03005418[2];
u8 *sub_08216E54(u16);
struct ResourceRef *sub_0821A2C4(u16);
void *Video_GetBackgroundMap(u32);
void sub_08215A74(u32,s32);
void sub_082160B8(u32,u32,void *,s32,s32,s32,s32,void *);
void sub_0821656C(u32,s32,s32,s32,s32);
void sub_08219574(u32,s32,s32);
s32 AffineBackground_Initialize(struct AffineBackground *p,u16 id,struct Config *c)
{
    struct Layer *layer;
    u8 *resource;
    s32 x,y,scale,tx,ty;
    void *map;
    gUnk_02000010[c->layer]=p;
    p->id=id;
    p->flags=c->flags;p->layer=c->layer;
    layer=&gUnk_03004C30[p->layer];
    p->v1C=c->resource;resource=sub_08216E54(p->v1C);
    if(p->flags&1){struct Dimensions *dims=sub_0821A2C4(0x56C2)->data;c->offsetX=dims->x;c->offsetY=dims->y;}
    if(p->flags&2){x=gUnk_03005418[0]+gUnk_030042A0.offsetX;y=gUnk_03005418[1]+gUnk_030042A0.offsetY;scale=c->scale;}
    else{x=c->x;y=c->y;scale=c->scale;}
    p->x=x<<4;p->y=y<<4;p->centerX=p->x;p->centerY=p->y;p->targetX=p->x;p->targetY=p->y;
    p->speedX=c->speedX;p->speedY=c->speedY;p->accumulatedX=0;p->accumulatedY=0;
    *(s16 *)(p->pad3C)=c->offsetX;*(s16 *)(p->pad3C+2)=c->offsetY;
    p->width=c->width<<3;p->height=c->height<<3;
    p->moveActive=0;p->moveDuration=0;p->moveTime=0;p->enabled=1;
    map=Video_GetBackgroundMap(p->layer);
    if(resource[0]=='M' && resource[1]=='P'){layer->flags=1;sub_08215A74(p->layer,c->mode);}
    else sub_082160B8(p->layer,1,resource,0,(s16)c->offsetX,(s16)c->offsetY,c->mode,map);
    p->speedScale=scale;
    {s32 drawX=p->x>>4,drawY=p->y>>4;
    tx=Div(drawX-128,8);ty=Div(drawY-128,8)&~1;
    sub_0821656C(p->layer,tx,ty,drawX,drawY);
    }
    p->fadeActive=0;
    {s32 blendX=c->blendX,blendY; p->blendX=blendX<<8;
    blendY=c->blendY;p->blendY=blendY<<8;
    p->stepX=0;p->stepY=0;
    p->targetBlendX=p->blendX;p->targetBlendY=p->blendY;
    p->stepX=p->blendX;p->stepY=p->blendY;
    if(c->fade)sub_08219574(p->layer,blendX,blendY);
    }
    return 0;
}
