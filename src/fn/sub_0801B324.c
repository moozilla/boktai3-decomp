#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define SelectionMenu_Initialize sub_0801B324
typedef struct {u32 words[8];} Resource;
Resource *sub_0821A520(int,int);
void sub_082196C4(void *,void *);
int sub_0821ABA8(int,int);int Script_SeekToKeyword(int);u32 sub_08227E90(void);
void sub_0801A218(void *);void sub_0801A418(void *);void sub_0801A6D0(void *);void sub_0801A3C0(void *);void sub_0801A470(void *);
void sub_08030F20(int,int,int,int);void sub_0801ABEC(void *);
void sub_08019F18(void *,void (*)(void));void sub_0801ADC0(void);void sub_08033568(void);
extern u32 gUnk_0300523C;
int SelectionMenu_Initialize(u8 *p) {
 Resource *resource=sub_0821A520(0xcb05,0x530d);
 if(!resource)goto fail;
 asm("" ::: "r0");
 *(Resource *)(p+0x4c)=*resource;sub_082196C4(p+0x4c,resource);
 resource=sub_0821A520(0xcb05,0x5d04);
 if(!resource)goto fail;
 asm("" ::: "r0");
 *(Resource *)(p+0x6c)=*resource;sub_082196C4(p+0x6c,resource);
 *(u16 *)(p+0x40)=sub_0821ABA8('c',0);
 *(u16 *)(p+0x42)=sub_0821ABA8('e',0);
 if(!Script_SeekToKeyword('w'))goto fail;
 if(!(*(u32 *)(p+0xda8)=sub_08227E90()))goto fail;
 if(!Script_SeekToKeyword('a'))goto fail;
 if((*(u32 *)(p+0xdac)=sub_08227E90()))goto configure;
 fail:return -1;
 configure:
 if(!Script_SeekToKeyword('k'))goto fail;
 if(!(*(u32 *)(p+0xdb0)=sub_08227E90()))goto fail;
 sub_0801A218(p);sub_0801A418(p);sub_0801A6D0(p);sub_0801A3C0(p);sub_0801A470(p);
 sub_08030F20(0,16,30,2);sub_0801ABEC(p);sub_08019F18(p,sub_0801ADC0);sub_08033568();
 {int mask=4;gUnk_0300523C|=mask;}return 0;
}
