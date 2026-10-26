#include "scene_actor_script.h"
struct SceneActorScriptPayloadView {
    u8 unknown0[8];
    u8 kind;
    u8 unknown9[3];
    u32 payload[2];
};
void EffectQuad_StartRemoteEntry(u32, const void *);
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
void SceneActorScript_DispatchKindPayload(void *input)
{
    struct SceneActorScriptPayloadView *descriptor = input;
    u32 kind;
    if (!Test(gEngineFlags, 0x4800))
        return;
    switch (descriptor->kind) {
        case 0:
            kind = 0;
            break;
        case 1:
            kind = 1;
            break;
        case 2:
            kind = 2;
            break;
        case 3:
            kind = 3;
            break;
        default:
            return;
    }
    EffectQuad_StartRemoteEntry(kind, descriptor->payload);
}
