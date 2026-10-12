#include "global.h"
#define SceneMessages_Create sub_08020C84
extern void *gUnk_0200047C;
void *sub_08219FBC(int,int);void sub_0821A04C(void*,void*,void*);int sub_08020C40(void*,int);void sub_0821A0C0(void*);void sub_08020B8C(void);void sub_08020C28(void);
void *SceneMessages_Create(int id){void*p;if(gUnk_0200047C)return 0;p=sub_08219FBC(2,0xac);if(p){sub_0821A04C(p,sub_08020B8C,sub_08020C28);if(sub_08020C40(p,id)<0){sub_0821A0C0(p);return 0;}}return p;}
