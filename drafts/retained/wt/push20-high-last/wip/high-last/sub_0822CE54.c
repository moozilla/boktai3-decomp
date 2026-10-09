#include "global.h"
struct Entry {u32 x;u16 limit;u8 pad[10];};extern const struct Entry gUnk_08E61280[];
s32 sub_0822CDA8(s32);void sub_0822CDEC(s32,s32),sub_0822CCE0(s32,u32);
u32 sub_0822CE54(s32 kind,s32 value) {
 s32 i=0;const struct Entry *entry=&gUnk_08E61280[kind];
 for(;i<16;i++) {
 if(sub_0822CDA8(i)<0) {
 sub_0822CDEC(i,kind);
 if(value<0) value=0;
 if(entry->limit*32<(value&0x7fff)) value=0;
 sub_0822CCE0(i,value);return 1;
 }
 }
 return 0;
}
