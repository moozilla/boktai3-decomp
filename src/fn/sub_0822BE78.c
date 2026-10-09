#include "global.h"
struct State {u8 pad[8];u8 slot;};extern struct State *gUnk_030053F8;
void sub_0822BB60(void),sub_0821B148(void);
u8 sub_0822BC6C(u32),sub_0822BBD8(u32),sub_0822BCE4(u32);
u32 sub_0822BE78(void) {
 u32 slot=gUnk_030053F8->slot,other=1-slot;
 sub_0822BB60();
 if(!sub_0822BC6C(slot)) return 0;
 sub_0821B148();
 if(!sub_0822BBD8(other)) return 0;
 if(!sub_0822BCE4(other)) return 0;
 return 1;
}
