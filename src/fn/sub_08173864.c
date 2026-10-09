#include "global.h"
static inline void put(s16 *p,s32 x,s32 y){s32 zero=0;p[0]=x;p[1]=y;p[2]=zero;p[3]=zero;}
void sub_08173864(s16 *out,u8 *in){
 u32 type=in[3];
 if(type<=15){s32 x=in[1],y=in[2];put(out,x*24+112,y*24+32);}
 else{
 if(type==39){put(out,216,80);}
 else if(type==38){put(out,216,56);}
 else if(type==36||type==40){put(out,216,104);}
 else if(type==32){put(out,216,type);}
 else{out[0]=144;out[1]=32;}
 }
}
