#include "global.h"
#define SceneActors_CopyAvailablePosition sub_08021BC0
typedef struct{int a,b;} Position;
u8*sub_0802141C(void);
int SceneActors_CopyAvailablePosition(int*out){u8*p=sub_0802141C();int result;if(p){*(Position*)out=*(Position*)(p+0x54);result=0;}else result=-1;return result;}
