#include "global.h"
extern const u32 gUnk_08603300[];

u32 Script_SearchActorRegistry(u32, u32, u32, u32);

u32 Script_GetActorFactory(u32 a)
{
    return Script_SearchActorRegistry(a, (u32)gUnk_08603300, 0, *(u16 *)0x030025F4);
}
