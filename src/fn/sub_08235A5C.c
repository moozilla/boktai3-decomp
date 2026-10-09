#include "global.h"
struct Pair { u32 a,b; };
struct Elem { u8 pad[0x1c]; struct Pair coords; u8 tail[8]; };
struct Owner { u8 pad[0xe8]; struct Elem elems[2]; u8 resource[1]; };
void sub_082151E4(void *,u32);
void sub_082144A4(void *,void *,u32);
void sub_08215284(void *,u32);
void sub_08235A5C(struct Owner *p)
{
    s32 i;
    sub_082151E4(p->resource,0x24ba);
    for(i=0;i<2;i++) {
        sub_082144A4(&p->elems[i],p->resource,1);
        sub_08215284(p->resource,0x2e5);
        p->elems[i].coords=*(struct Pair *)(*(u8 **)((u8 *)p+0x18)+0x30);
    }
}
