#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "background_controller.h"
#define Background_Initialize sub_08001AE4
struct Config { u8 flags,layer; u16 resource; s32 x,y,scrollX,scrollY,rateX,rateY,limitX,limitY,centerX,centerY,offsetX,offsetY,mode,priority,blendX,blendY; };
struct Layer { u16 flags; u8 pad[26]; s16 halfWidth,halfHeight,x,y; u8 tail[12]; };
struct Camera { s16 x,y,a,b,offsetX,offsetY; };
struct Dimensions { s16 pad[4],x,y; };
struct ResourceRef { u32 tag; struct Dimensions *data; };
extern struct Background *gUnk_02000000[];
extern struct Layer gUnk_03004C30[];
extern struct Camera gUnk_030042A0;
extern s16 gUnk_03005418[2];
u8 *sub_08216E54(u16);
struct ResourceRef *sub_0821A2C4(u16);
void sub_08215A74(u32,s32);
void *Video_GetBackgroundMap(u32);
void sub_082160B8(u32,u32,void *,s32,s32,s32,s32,void *);
static inline u8 modeFlag(struct Background *p) { return p->flags & 16; }
s32 Background_Initialize(struct Background *p,u16 id,struct Config *c)
{
    struct Layer *layer;
    u8 *resource;
    register s32 offsetX asm("r9"), offsetY asm("r10");
    s32 x,y;
    u32 mode;
    gUnk_02000000[c->layer]=p;
    p->id=id;p->flags=c->flags;p->layer=c->layer;
    layer=&gUnk_03004C30[p->layer];
    p->resource=c->resource;
    resource=sub_08216E54(p->resource);
    if(p->flags&1) {struct Dimensions *dims=sub_0821A2C4(0x56C2)->data;offsetX=dims->x;offsetY=dims->y;}
    else {offsetX=0;offsetY=0;}
    if(p->flags&2){x=gUnk_03005418[0]+gUnk_030042A0.offsetX;y=gUnk_03005418[1]+gUnk_030042A0.offsetY;}
    else {x=c->x;y=c->y;}
    p->rateX=c->rateX;p->rateY=c->rateY;
    layer->x=x-120;layer->y=y-80;
    p->x=x<<7;p->y=y<<7;p->startX=p->x;p->startY=p->y;p->targetX=p->x;p->targetY=p->y;
    p->scrollX=c->scrollX;p->scrollY=c->scrollY;p->unknown38=0;p->unknown3C=0;
    p->offsetX=offsetX>>7;p->offsetY=offsetY>>7;
    p->speedX=c->offsetX;p->speedY=c->offsetY;
    p->limitX=c->limitX;p->limitY=c->limitY;p->centerX=c->centerX;p->centerY=c->centerY;
    p->moveActive=0;p->moveDuration=0;p->moveTime=0;p->fadeActive=0;
    p->blendX=c->blendX<<8;p->blendY=c->blendY<<8;
    p->blendStepX=0;p->blendStepY=0;
    p->blendTargetX=p->blendX;p->blendTargetY=p->blendY;
    p->blendStepX=p->blendX;p->blendStepY=p->blendY;
    mode=modeFlag(p)!=0;
    if(resource[0]=='M' && resource[1]=='P') {layer->flags=mode;sub_08215A74(p->layer,c->mode);}
    else {void *map=Video_GetBackgroundMap(p->layer);
        u32 layerId=p->layer;
        register u32 drawMode asm("r1")=mode;
        sub_082160B8(layerId,drawMode,resource,0,offsetX,offsetY,c->mode,map);
    }
    if(p->limitX && p->limitY){layer->halfWidth=p->limitX/2;layer->halfHeight=p->limitY/2;}
    return 0;
}
