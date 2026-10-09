#include "global.h"
void sub_0817373C(void *,int);void sub_08173C0C(void);void sub_08163EB8(void *,void (*)(void),int);
void sub_0816CDB8(void *,int,int);void sub_081663A0(void *,int,int,int,int);
void sub_081708CC(void *);void sub_0816D920(void *,int);void sub_08173470(void *);
void sub_0816701C(void *,int,int,int,int,int);void sub_08166B7C(void *,int);
void sub_08173928(u8 *p,int flag) {
 int z;u8 *a;
 sub_0817373C(p,0);sub_08163EB8(p,sub_08173C0C,1);sub_0816CDB8(p+0x4384,1,0);
 z=0;sub_081663A0(p,1,0,0,z);if(flag)sub_081708CC(p);
 a=p+0x1620;sub_0816D920(a,p[0x4387]);sub_08173470(p);
 sub_0816701C(p,0,1,0,z,z);sub_08166B7C(p,2);
}
