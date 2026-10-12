#include "global.h"
#define Presentation_ClearTilemap sub_081E2D74
#define Presentation_ConfigureLayout sub_081E2F44
// Provisional name: Presentation_ConfigureLayout
struct Sequence {u8 p0[0x18];u8 resource[0x20];u8 layer0[8];u32 flags0;u8 p1[0x98-0x44];u8 layer1[8];u32 flags1;u8 p2[0xb8-0xa4];s16 x1,y1;u8 p3[0xf8-0xbc];u8 layer2[8];u32 flags2;u8 p4[0x158-0x104];u8 layer3[8];u32 flags3;u8 p5[0x178-0x164];s16 x3,y3;u8 p6[0x1b8-0x17c];u8 layer4[8];u32 flags4;u8 p7[0x21c-0x1c4];u16 option;u8 p8[2];u16 timer;u8 p9[0x234-0x222];void(*callback)(void*);};
void sub_0821980C(void*,void*,s32,s32);void sub_08220D78(void*,void*,s32,s32,s32);void sub_081E28D4(void*);void Presentation_ClearTilemap(void);
// Configure the presentation layers for the selected scripted sequence layout.
void Presentation_ConfigureLayout(struct Sequence*p){
 p->flags0&=~1;
 switch(p->option){
 case 0:p->flags0|=1;break;
 case 1:sub_0821980C(p->layer0,p->resource,15,1);break;
 case 2:sub_0821980C(p->layer0,p->resource,16,1);break;
 case 3:case 4:sub_0821980C(p->layer0,p->resource,17,1);break;
 case 5:sub_0821980C(p->layer0,p->resource,18,1);break;
 case 6:sub_0821980C(p->layer0,p->resource,19,1);break;
 case 7:case 8:sub_0821980C(p->layer0,p->resource,20,1);break;
 case 9:case 10:case 11:sub_0821980C(p->layer0,p->resource,21,1);break;
 case 12:sub_0821980C(p->layer0,p->resource,22,1);break;
 case 13:sub_0821980C(p->layer0,p->resource,23,1);break;
 default:p->flags0|=1;break;
 }
 switch(p->option){
case 0:
 p->flags1&=~1;sub_08220D78(p->layer1,p->resource,0,1,0);
 p->flags2&=~1;sub_08220D78(p->layer2,p->resource,2,1,0);
 p->flags4&=~1;sub_08220D78(p->layer4,p->resource,3,1,0);
 p->flags3|=1;
 p->callback=sub_081E28D4;break;
case 1:
 p->flags1&=~1;sub_08220D78(p->layer1,p->resource,0,1,0);
 p->flags2&=~1;sub_08220D78(p->layer2,p->resource,2,1,0);
 p->flags4&=~1;sub_08220D78(p->layer4,p->resource,3,1,0);
 p->flags3&=~1;sub_0821980C(p->layer3,p->resource,0,1);
 p->callback=sub_081E28D4;break;
case 4:case 12:
 p->flags1&=~1;sub_08220D78(p->layer1,p->resource,1,1,0);
 p->flags2&=~1;sub_08220D78(p->layer2,p->resource,2,1,0);
 p->flags4&=~1;sub_08220D78(p->layer4,p->resource,3,1,0);
 p->flags3|=1;
 p->callback=sub_081E28D4;break;
case 2:case 9:
 p->flags1&=~1;sub_08220D78(p->layer1,p->resource,0,1,0);
 p->flags2&=~1;sub_08220D78(p->layer2,p->resource,2,1,0);
 p->flags4&=~1;sub_08220D78(p->layer4,p->resource,3,1,0);
 p->flags3|=1;
 p->callback=sub_081E28D4;break;
case 3:
 p->flags1&=~1;sub_08220D78(p->layer1,p->resource,0,1,0);
 p->x1=60;p->y1=-30;p->flags2|=1;p->flags4|=1;
 sub_0821980C(p->layer3,p->resource,1,1);
 p->x3=-65;p->y3=40;p->flags3|=1;p->callback=0;break;
 default:p->flags1|=1;p->flags2|=1;p->flags4|=1;p->flags3|=1;Presentation_ClearTilemap();p->callback=0;break;
 }
 p->timer=0;
}
