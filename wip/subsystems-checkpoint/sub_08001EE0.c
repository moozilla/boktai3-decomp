#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define Background_LoadMapsFromScript sub_08001EE0
struct Resource { u8 signature[2]; u16 unk2,count,unk6,unk8,unkA; u32 maps, tiles, extra; };
s32 sub_0821ABA8(s32,s32);
s32 Script_SeekToKeyword(s32);
u8 *Script_GetPc(void);
s32 sub_0821AA00(u8 *);
u8 *sub_08216E54(u16);
s32 sub_08001EB4(struct Resource *,u16);
void sub_082161B4(s32,s32,void *,s16,s16,s32,s32 *);
static inline s32 nextShift(void) { u8 *p=Script_GetPc(); return p?sub_0821AA00(p)<<7:0; }
s32 Background_LoadMapsFromScript(void)
{
    s32 indices[4];
    struct Resource resource;
    register u8 *base asm("r6")=sub_08216E54(sub_0821ABA8('d',0));
    register volatile s32 target asm("r10"), x asm("r8"), y asm("r7"), n asm("r9"), explicitIndices asm("r5");
    s32 i;
    resource=*(struct Resource *)base;
    resource.maps+=(u32)base;
    resource.tiles+=(u32)base;
    resource.extra+=(u32)base;
    target=sub_0821ABA8('t',1);
    if(Script_SeekToKeyword('o')) {u8 *cursor; x=nextShift(); cursor=Script_GetPc(); if(cursor)y=sub_0821AA00(cursor)<<7;else y=0;} else {x=0;y=0;}
    n=sub_0821ABA8('n',1);
    explicitIndices=0;
    if(Script_SeekToKeyword('i')) {
        u8 *p=Script_GetPc();
        s32 *out;
        explicitIndices=1;
        out=indices;
        do {
            if(p) {*out=sub_0821AA00(p);p=Script_GetPc();}
            else *out=(s32)p;
            out++;
        } while((s32)out<=(s32)(indices+3));
    } else {
        s32 *out=indices+3;
        for(i=3;i>=0;i--) {*out=i;out--;}
    }
    if(!explicitIndices && Script_SeekToKeyword('c')) {
        u8 *p=Script_GetPc();
        s32 *out=indices;
        for(i=3;i>=0;i--) {
            if(p) {*out=sub_08001EB4(&resource,sub_0821AA00(p));p=Script_GetPc();}
            out++;
        }
    }
    if(base[0]=='M' && base[1]=='P') sub_082161B4(target,0,base,x,y,n,indices);
}
