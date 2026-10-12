#include "global.h"
/* Tentative role: split an effect mode into two selector values. */
static inline s32 variant(s32 v) {
 switch(v) { case 0:return 0; case 1:return 1; case 2:return 2; default:return 1; }
}
static inline s32 group(u32 v) {
 if(v<=2) return 0;
 if(v-3<=1) return 2;
 if(v==5) return 3;
 return 1;
}
void sub_0813C000(u32 mode,u32 *a,u32 *b) { *a=variant(mode&3); *b=group(mode); }
