#include "global.h"
struct V {u32 x:16,y:16,z:16,w:16;};
s32 Script_SeekToKeyword(s32);s32 Script_GetValue(void);void sub_08226AE4(void *,void *);
void sub_08226B50(void) {
 struct V a,b;
 if(Script_SeekToKeyword(0x49)) {
 a.x=Script_GetValue();a.y=Script_GetValue();a.z=Script_GetValue();
 if(Script_SeekToKeyword(0x41)) {
 b.x=Script_GetValue();b.y=Script_GetValue();b.z=Script_GetValue();sub_08226AE4(&a,&b);
 }
 }
}
