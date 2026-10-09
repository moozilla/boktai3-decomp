#include "global.h"
typedef struct { u8 pad[0x38]; u32 word; u16 pad2; u16 h3e; u32 pad3; u16 h44; } Src;
typedef struct { u8 pad[0x124]; u32 word; u16 h128; u8 pad2[4]; u16 h12e; } Dst;
void sub_0821FF8C(Src *, Src *, Dst *);
void sub_08159860(Src *, Src *, s32);
void sub_081A6EF8(Src *a, Src *b, Dst *c) {
    u32 word;
    sub_0821FF8C(a, b, c);
    c->h128 = b->h3e;
    word = a->word;
    c->word = word;
    c->h12e = a->h44;
    sub_08159860(a, b, 10);
}
