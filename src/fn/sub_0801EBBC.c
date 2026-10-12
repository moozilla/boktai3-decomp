#include "global.h"
#define SceneObjects_ScriptSetSelection sub_0801EBBC
u8 *sub_0821A2C4(int);int Script_SeekToKeyword(int);int Script_GetValue(void);
void SceneObjects_ScriptSetSelection(void){u8*p=sub_0821A2C4(0x85e6);if(p&&Script_SeekToKeyword(0x6e))*(u16*)(p+0x2e)=Script_GetValue();}
