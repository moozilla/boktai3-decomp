#include "global.h"
struct Resource {u32 words[8];};
struct Drawing {u8 pad0[0x60];};
struct Position {unsigned int x:16;unsigned int y:16;unsigned int z:16;unsigned int pad:16;};
struct Display {
  u8 pad0[0x18];
  u32 timer;
  u8 state,entered,finished,pad1f;
  struct Resource resource;
  struct Drawing drawings[4];
  struct Resource extraResource;
  struct Drawing extras[2];
};
struct Resource *sub_0821A520(int,int);
void sub_082196C4(void*,void*);
void sub_0821983C(void*,void*,int,int,int,int,int,const void*);
int sub_0803AB28(struct Display *p) {
  struct Resource *resource;
  struct Drawing *drawing;
  struct Position position;
  int i;
  resource=sub_0821A520(0xcb05,0x7c8b);
  if(!resource)return -1;
  p->resource=*resource;
  sub_082196C4(&p->resource,resource);
  drawing=p->drawings;
  i=3;
  do {
    sub_0821983C(drawing,&p->resource,24,17,0,0,60,0);
    i--;drawing++;
  } while(i>=0);
  resource=sub_0821A520(0xcb05,0x5d04);
  if(!resource)return -1;
  p->extraResource=*resource;
  sub_082196C4(&p->extraResource,resource);
  drawing=p->extras;
  i=0;
  do {
    int animation;
    if(i==0){position.x=132;animation=405;}
    else {position.x=220;animation=408;}
    position.y=768;position.z=0;
    sub_0821983C(drawing,&p->extraResource,animation,0x10010,0,0,60,&position);
    drawing++;i++;
  } while(i<=1);
  p->state=0;p->entered=1;p->timer=0;p->finished=0;
  return 0;
}
