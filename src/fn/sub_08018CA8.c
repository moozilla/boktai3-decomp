#include "global.h"
void*sub_08219FBC(int,int);void sub_0821A04C(void*,void*,void*);int sub_08018898(void*);int sub_080189A8(void*);int sub_080189C8(void*,void*,int,int,int,int);void sub_0821A0C0(void*);int Script_SeekToKeyword(int);int Script_GetValue(void);int sub_0821ABA8(int,int);
struct Vec{signed x:16; signed y:16;signed z:16;};
#define OrbitingParticles_CreateFromScript sub_08018CA8
void*OrbitingParticles_CreateFromScript(void){void*p=sub_08219FBC(11,436);struct Vec pos;int radius,duration,count,type;if(p){if(Script_SeekToKeyword('p')){pos.x=Script_GetValue();pos.y=Script_GetValue();pos.z=Script_GetValue();}else{pos.x=0;pos.y=0;pos.z=0;}radius=sub_0821ABA8('d',256);duration=sub_0821ABA8('t',256);count=sub_0821ABA8('n',8);type=sub_0821ABA8('T',0);sub_0821A04C(p,sub_08018898,sub_080189A8);if(sub_080189C8(p,&pos,radius,duration,count,type)<0){sub_0821A0C0(p);return 0;}}return p;}
