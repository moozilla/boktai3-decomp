#include "global.h"
void sub_08020D68(void *, u32);
struct S { u8 f0[0xaa]; u8 aa; u8 ab,ac,ad,ae; u8 af; u8 b0; u8 f2[0x13]; u32 c4; };

static inline u8 chk(struct S *p) {
    if (p->b0) {
        p->b0 = 0;
        p->af = 0;
        return 1;
    }
    return 0;
}

static inline void action(struct S*p,int mode,int zero,int id){p->ac=mode;p->ad=zero;p->ae=id;p->c4=zero;p->b0=1;}
void sub_0802B240(struct S*p){if(chk(p)){u32 k=16;p->aa=k;}if(p->af){action(p,2,0,4);sub_08020D68((u8*)p+0x118,1);}else p->c4++;}
