#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define Transformation_Charge sub_0801BA0C
typedef struct {s16 x,y,z,pad;} Vec;
typedef struct {u8 pad[48];Vec position;} Actor;
extern Actor *gUnk_02000580;
int sub_0801B54C(void *);
void sub_0801C1E8(void *,int,int,void *);
void sub_0815BE5C(void *,int);void sub_0822B2F8(int);
void sub_08015B84(int,int,int,int,void *);
void sub_0801B534(void *,void (*)(void));void sub_0801BAA8(void);
void Transformation_Charge(u8 *p) {
 Actor *actor=gUnk_02000580;register Actor *positionActor asm("r6")=actor;
 u16 param;Vec pos;
 asm("" : "+r"(positionActor));
 if(sub_0801B54C(p)) {
  param=2;sub_0801C1E8(p,1,1,&param);
  sub_0815BE5C(actor,3);sub_0822B2F8(0x2e1);
 }
 if(*(u32 *)(p+0x3c)-30<=69 && !(*(u32 *)(p+0x3c)&3)) {
  {Vec *out=&pos;asm("" : "+r"(out));pos=positionActor->position;out->x+=200;
  sub_08015B84(2,0,0x2000,64,out);}sub_0822B2F8(0x1c5);
 }
 if(*(u32 *)(p+0x3c)>99)sub_0801B534(p,sub_0801BAA8);
 else (*(u32 *)(p+0x3c))++;
}
