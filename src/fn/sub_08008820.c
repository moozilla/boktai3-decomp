#include "global.h"
struct Vec {s32 x:16;s32 y:16;s32 z:16;s32 pad:16;};
s32 Script_SeekToKeyword(s32);s32 Script_GetValue(void);s32 Script_GetPc(void);s32 sub_0821ABA8(s32,s32);
s32 sub_080084F4(struct Vec *,s32,s32,s32,s32,s32,s32,s32 *,s32,s32,s32);
#define Trigger_CreateContainerFromScript sub_08008820
s32 Trigger_CreateContainerFromScript(void){
 s32 arguments[3];struct Vec position;s32 flip,id,kind,startScript,script,bob,sound,hidden,condition; s32 *out,count;
 if(Script_SeekToKeyword('s')){position.x=Script_GetValue();position.y=Script_GetValue();position.z=Script_GetValue();}
 else{position.x=0;position.y=0;position.z=0;}
 if(Script_SeekToKeyword('I')){id=Script_GetValue();kind=Script_GetValue();}else{id=0;kind=0;}
 flip=sub_0821ABA8('d',0);startScript=sub_0821ABA8('g',0);script=sub_0821ABA8('p',0);bob=sub_0821ABA8('m',0);sound=sub_0821ABA8('M',0);hidden=sub_0821ABA8('S',0);condition=sub_0821ABA8('c',0);
 {s32 zero=0; s32 *clear=arguments+2;do{*clear--=zero;}while((s32)clear>=(s32)arguments);}
 if(Script_SeekToKeyword('a')){count=0;out=arguments;while(Script_GetPc()&&count<3){*out++=Script_GetValue();count++;}}
 return sub_080084F4(&position,id,kind,flip,bob,startScript,script,arguments,sound,hidden,condition);
}
