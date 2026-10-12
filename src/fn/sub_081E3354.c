#include "global.h"
struct Sequence {u8 pad[0x218];u32 resource;u16 option;u8 gap[6];u16 first,last,timer,duration;};
s32 Script_SeekToKeyword(s32);s32 Script_GetValue(void);u32 sub_08227E90(void);
void sub_080311B0(s32);void sub_08030C84(u32);void sub_08030F20(s32,s32,s32,s32);void sub_08030B78(s32);void sub_08030DBC(s32);
// Configure the scripted frame range and its duration.
void sub_081E3354(struct Sequence *p){
 if(Script_SeekToKeyword('r')){
 p->resource=sub_08227E90();
 if(Script_SeekToKeyword('m')){
 p->first=Script_GetValue();p->last=p->first+Script_GetValue()-1;
 sub_080311B0(0x81b);sub_08030C84(p->resource);
 if(!p->option)sub_08030F20(1,0,30,10);
 sub_08030B78(1);sub_08030DBC(p->first);
 if(Script_SeekToKeyword('c'))p->duration=Script_GetValue();else p->duration=60;
 p->timer=0;
 }
 }
}
