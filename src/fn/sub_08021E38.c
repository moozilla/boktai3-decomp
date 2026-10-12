#include "global.h"
#define SceneActors_ScriptSetEffects sub_08021E38
u8*sub_0802141C(void);int Script_SeekToKeyword(int);void sub_0822B2F8(int);
void SceneActors_ScriptSetEffects(void){u8*p=sub_0802141C();if(p){if(Script_SeekToKeyword('s')){p[0x154]=1;sub_0822B2F8(0x1c3);}if(Script_SeekToKeyword('e'))p[0x154]=0;if(Script_SeekToKeyword('b'))p[0x155]=1;if(Script_SeekToKeyword('i'))p[0x156]=1;}}
