#include "global.h"
void *sub_08219FBC(u32,u32);void sub_0821A04C(void*,void(*)(void*),void(*)(void*));void sub_0821A0C0(void*);
void sub_0822C07C(void*),sub_0822C080(void*);s32 sub_0822C098(void*,u32,u32);
void *sub_0822C0CC(u32 a,u32 b) {
 void *obj=sub_08219FBC(4,88);
 if(obj) {
 sub_0821A04C(obj,sub_0822C07C,sub_0822C080);
 if(sub_0822C098(obj,a,b)<0) {sub_0821A0C0(obj);return 0;}
 }
 return obj;
}
