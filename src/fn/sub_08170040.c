#include "global.h"
int sub_08165A30(void *);void sub_0822B2F8(int);void sub_0816DAF8(void *);
void sub_0822D0F0(int);void sub_0822D188(int);
struct S {u8 pad[0x4388];u8 a,b,c;u8 pad2[0x4860 - 0x438b];u8 d;};
void sub_08170040(struct S *p) {
 int r=sub_08165A30(p);
 if(r==0) {sub_0822B2F8(0xde);sub_0816DAF8(p);}
 else if(r==1) {sub_0822B2F8(0xdd);if(!p->d)sub_0822D0F0(0);else sub_0822D188(0);
 p->a=0;p->b=0;p->c=0;sub_0816DAF8(p);}
}
