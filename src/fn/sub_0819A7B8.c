#define SceneEffect_CreateLinkedEntry sub_0819A7B8
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
void sub_0819A698(struct SceneEffectLink *);
void sub_081FE92C(struct EffectEntryConfig *);
s32 sub_081FE760(struct EffectEntryConfig *, void *);
void sub_081FEA80(s32);
void sub_0819A7B8(struct SceneEffectLink *p)
{
    struct EffectEntryConfig config;
    s32 entry;
    p->entry = 0xFF;
    p->secondary = -1;
    sub_0819A698(p);
    sub_081FE92C(&config);
    config.flags = 0;
    config.kind = 1;
    config.unknown8 = 0;
    entry = sub_081FE760(&config, p->entryData);
    if (entry >= 0) {
        sub_081FEA80(entry);
        p->entry = entry;
    }
    p->flags |= -1;
}
