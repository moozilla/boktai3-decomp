// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define SceneObject_BeginRaising sub_0801F3A0
void sub_0822B2F8(int);void sub_0801F430(void);
void SceneObject_BeginRaising(u8*s){sub_0822B2F8(0x152);s[0x39]=0;s[0x3a]=0;{int two=2;asm("" : "+r"(two));s[0x37]=two;}*(void**)(s+0x30)=sub_0801F430;}
