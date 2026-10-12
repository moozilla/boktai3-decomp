#include "global.h"
/* Tentative role: select an effect flag from actor and global state. */
typedef struct State { u8 pad0[0x20]; u32 flags; u8 pad24[0x418-0x24]; u8 mode; } State;
extern u8 *gUnk_02000710;
static inline u8 global_mode(void) { u32 mask=2; if(*(u32 *)(gUnk_02000710+0x868)&mask) return 1; return 0; }
s32 sub_0813C8F0(State *s) {
 s32 result=0;
 if(s->mode==2) return 0;
 if(!global_mode()) { if(s->flags&16) result=8; }
 else { if(s->flags&16) result=4; }
 return result;
}
