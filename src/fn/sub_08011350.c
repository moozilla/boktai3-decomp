#include "global.h"
struct Position{s32 x:16,y:16,z:16,pad:16;};
void sub_0821983C(void*,void*,int,int,int,int,int,void*);
#define MessageMenu_InitializeTransition sub_08011350
void MessageMenu_InitializeTransition(u8 *m){struct Position zero;zero.x=0;zero.y=0;zero.z=0;sub_0821983C(m+0x478,m+0x458,57,16,0,0,60,&zero);}
