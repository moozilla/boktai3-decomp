#include "global.h"
#define SceneTilemap_FillRectangle sub_0806F1D0
u16 *sub_0806F1B0(s32,u32,u32);
void SceneTilemap_FillRectangle(s32 column,s32 row,s32 width,s32 height,s32 tile,s32 layer)
{
 s32 i;
 for(i=0;i<height;){
  u16 *p=sub_0806F1B0(layer,column,row+i);
  s32 next=i+1;
  if(width>0){s32 n=width;do{*p++=tile;}while(--n);}
  i=next;
 }
}
