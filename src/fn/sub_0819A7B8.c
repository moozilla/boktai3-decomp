#include "global.h"
struct SceneEffectLink {
    u8 unknown[0x24];
    u8 entryData[0x10];
    u8 entry;
    s8 secondary;
    u8 pad36[5];
    u8 flags;
};
struct EffectEntryConfig {
    u32 unknown0;
    u16 kind;
    u8 unknown6;
    u8 flags;
    u32 unknown8;
    u8 rest[0x14];
};
void LinkEffect_UpdateEndpoints(struct SceneEffectLink *);
void RisingActorConfig_Initialize(struct EffectEntryConfig *);
s32 SunOrb_CreateAtPosition(struct EffectEntryConfig *, void *);
void RisingActorManager_BeginRaise(s32);
void SceneEffect_CreateLinkedEntry(struct SceneEffectLink *p)
{
    struct EffectEntryConfig config;
    s32 entry;
    p->entry = 0xFF;
    p->secondary = -1;
    LinkEffect_UpdateEndpoints(p);
    RisingActorConfig_Initialize(&config);
    config.flags = 0;
    config.kind = 1;
    config.unknown8 = 0;
    entry = SunOrb_CreateAtPosition(&config, p->entryData);
    if (entry >= 0) {
        RisingActorManager_BeginRaise(entry);
        p->entry = entry;
    }
    p->flags |= -1;
}
