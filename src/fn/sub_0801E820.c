#include "global.h"
#define SceneObjects_ScriptDeactivate sub_0801E820
u8 *sub_0821A2C4(int);int Script_SeekToKeyword(int);int Script_GetValue(void);void sub_0801F3CC(void);
int SceneObjects_ScriptDeactivate(void){u8*p=sub_0821A2C4(0x85e6);int found,id,i;if(!p)return -1;found=0;if(!Script_SeekToKeyword(0x6e))return -1;id=Script_GetValue();for(i=0;i<*(u16*)(p+0x1a);i++){u8*s=*(u8**)(p+0x42c)+i*140;if(*(u16*)(s+0x34)==id){if(Script_SeekToKeyword(0x70))*(int*)(s+0x64)=Script_GetValue();if(s[0x37]==0||s[0x37]==2){s[0x37]=3;*(void**)(s+0x30)=sub_0801F3CC;}found=1;}}return found;}
