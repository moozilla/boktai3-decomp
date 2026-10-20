#include "global.h"
#define Hud_LoadLayoutResources sub_081779E8
struct Header {
    u32 words[8];
};
void *ScriptProc_Start(u32, u32);
void ResourceHeader_Relocate(void *, void *);
void *AnimData_FindEntry(u32);
void sub_08217DE0(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void Hud_LoadLayoutResources(u8 *p)
{
    struct Header *resource;
    u8 *effect;
    s32 i, origin;
    resource = ScriptProc_Start(0xcb05, 0x530d);
    if (!resource)
        return;
    *(struct Header *)(p + 0x64) = *resource;
    ResourceHeader_Relocate(p + 0x64, resource);
    resource = ScriptProc_Start(0xcb05, 0x5d04);
    if (!resource)
        return;
    *(struct Header *)(p + 0xa4) = *resource;
    ResourceHeader_Relocate(p + 0xa4, resource);
    resource = ScriptProc_Start(0xcb05, 0xa2db);
    if (!resource)
        return;
    *(struct Header *)(p + 0x84) = *resource;
    ResourceHeader_Relocate(p + 0x84, resource);
    resource = ScriptProc_Start(0xcb05, 0x32ba);
    if (!resource)
        return;
    *(struct Header *)(p + 0xc4) = *resource;
    ResourceHeader_Relocate(p + 0xc4, resource);
    resource = ScriptProc_Start(0xcb05, 0xde23);
    if (!resource)
        return;
    *(struct Header *)(p + 0xe4) = *resource;
    ResourceHeader_Relocate(p + 0xe4, resource);
    resource = ScriptProc_Start(0xcb05, 0xdfde);
    if (!resource)
        return;
    *(struct Header *)(p + 0x104) = *resource;
    ResourceHeader_Relocate(p + 0x104, resource);
    resource = ScriptProc_Start(0xcb05, 0xe2de);
    if (!resource)
        return;
    *(struct Header *)(p + 0x124) = *resource;
    ResourceHeader_Relocate(p + 0x124, resource);
    resource = ScriptProc_Start(0xcb05, 0x7c8b);
    if (!resource)
        return;
    *(struct Header *)(p + 0x144) = *resource;
    ResourceHeader_Relocate(p + 0x144, resource);
    resource = ScriptProc_Start(0xcb05, 0x2bec);
    if (!resource)
        return;
    *(struct Header *)(p + 0x164) = *resource;
    ResourceHeader_Relocate(p + 0x164, resource);
    *(void **)(p + 0x4cdc) = AnimData_FindEntry(0x1c1c);
    effect = p + 0x4ce0;
    origin = -4;
    for (i = 3; i >= 0; i--) {
        sub_08217DE0(effect, *(void **)(p + 0x4cdc), 128);
        Effect_SetOrigin(effect, origin, origin);
        *(u32 *)effect |= 17;
        effect[15] = 0;
        effect += 40;
    }
}
