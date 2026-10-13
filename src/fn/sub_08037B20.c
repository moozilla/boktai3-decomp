#include "global.h"
struct Panel  {
    u8 bytes[0x25c];
};
struct P  {
    u8 pad[0x19];
    u8 state;
    u8 pad1[0x12];
    struct Panel panels[2];
};
int sub_08036BEC(void);
void sub_08037A6C(struct P*);
int sub_0824923C(void*,const void*);
void sub_080375BC(struct P*,struct Panel*,int);
extern const void*gUnk_08605780[];
int sub_08037B20(struct P*p) {
    struct Panel*q;
    int i;
    if(!sub_08036BEC()) {
        sub_08037A6C(p);
        sub_0824923C(p,gUnk_08605780[p->state]);
        q=p->panels;
        i=0;
        do {
            sub_080375BC(p,q,i);
            i++;
            q++;
        }while(i<=1);
    }return 0;
}
