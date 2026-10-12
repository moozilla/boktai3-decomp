#include "global.h"
#define SceneObjects_ScriptActivate sub_0801E784
u8 *sub_0821A2C4(int);int Script_SeekToKeyword(int);int Script_GetValue(void);void sub_0801F3A0(void);
int SceneObjects_ScriptActivate(void){u8*p=sub_0821A2C4(0x85e6);int found,id,i;if(!p)return -1;found=0;if(!Script_SeekToKeyword(0x6e))return 0;id=Script_GetValue();for(i=0;i<*(u16*)(p+0x1a);i++){u8*s=*(u8**)(p+0x42c)+i*140;if(*(u16*)(s+0x34)==id){if(Script_SeekToKeyword(0x70))*(int*)(s+0x60)=Script_GetValue();if(s[0x37]==1||s[0x37]==3){s[0x37]=2;*(void**)(s+0x30)=sub_0801F3A0;}found=1;}}return found;}
