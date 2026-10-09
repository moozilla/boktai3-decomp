#include "global.h"
int sub_08165A30(void *);void sub_0822B2F8(int);void sub_0816D98C(void *);
void sub_0816FF4C(void);void sub_08163EB8(void *,void (*)(void),int);
void sub_0816CDB8(void *,int,int);void sub_0816D920(void *,int);
void sub_0816D868(void *);void sub_0816B614(void *);void sub_081672FC(void *);
void sub_08166B7C(void *,int);void sub_0816701C(void *,int,int,int,int,int);
void sub_081663A0(void *,int,int,int,int);void sub_08030BF8(void);void sub_0816FB0C(void *);
void sub_081700A8(u8 *p) {
 int r=sub_08165A30(p);u8 *q,*a;
 if(r==0) {
 sub_0822B2F8(0xde);sub_0816D98C(p);sub_08163EB8(p,sub_0816FF4C,1);
 q=p+0x4384;sub_0816CDB8(q,1,0);a=p+0x1620;sub_0816D920(a,p[0x4387]);
 sub_0816D868(p);sub_0816B614(q);sub_081672FC(p);sub_08166B7C(p,4);
 sub_0816701C(p,0,0,0,1,r);sub_081663A0(p,1,0,0,r);
 }else if(r==1) {sub_08030BF8();sub_0816FB0C(p);}
}
