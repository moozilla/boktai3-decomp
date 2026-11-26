#ifndef GUARD_SCENE_EVENT_TYPES_H
#define GUARD_SCENE_EVENT_TYPES_H
#include "global.h"

struct SceneEventCommand {
    u16 id;
    u8 receiverKind, trigger;
    u16 unknown4;
    u8 kind, argumentCount;
    s16 value;
    u8 parameterA, parameterB; /* command-specific bytes in the tail padding */
};
struct SceneEventReceiver {
    u16 id;
    u8 kind, trigger;
    u16 unknown4;
    u8 count[2];
    struct SceneEventCommand *pending[2][4];
};
#endif
