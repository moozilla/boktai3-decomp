#include "global.h"
#define TileAnimation_Initialize sub_08019698
typedef struct {u16 id,time;u8 active,count,frame,frames,unpaused,dirty,remove,pad;u16 source[8],dest[8],duration[8];} Channel;
typedef struct {u8 pad[24];u16 count,unused;Channel channels[8];void *tiles;} Manager;
extern Manager *gUnk_020000A4;
int sub_0821ABA8(int,int);
void *sub_08219C40(int);
void sub_08219DD8(void *,int);
void *sub_0821A520(int,int);
int sub_08215CB8(void *,int,void *,int,int);
int TileAnimation_Initialize(Manager *p) {
 int i,resource,mode;Channel *q;void *data;
 gUnk_020000A4=p;
 p->count=sub_0821ABA8('s',32);
 p->tiles=sub_08219C40(p->count*32);
 if(p->tiles) {
  sub_08219DD8(p->tiles,p->count*32);
  q=p->channels; { int zero=0; i=7; 
  do {q->active=zero;q->remove=zero;q->count=zero;q->id=zero;i--;q++;}while(i>=0); }
  resource=sub_0821ABA8('p',0);mode=sub_0821ABA8('m',0);
  data=sub_0821A520(0xceef,(u16)resource);
  if(data && sub_08215CB8(data,mode,p->tiles,0,p->count)>=0) return 0;
 }
 return -1;
}
