#ifndef GUARD_ACTOR_ABILITY_TYPES_H
#define GUARD_ACTOR_ABILITY_TYPES_H
#include "global.h"
struct ActorAbilityOwner;
/* Fourth-source configuration and runtime state. Configuration meanings vary by kind. */
struct ActorAbility {
    void (*callback)(void);
    s8 id;
    u8 kind;
    u16 interval, cost, parameter, duration;
    u8 unknownE[2];
    u8 enabled, request;
    u16 cooldown, remaining;
    u8 soundTimer, started;
    struct ActorAbilityOwner *owner;
};
struct ActorSecondaryAbility {
    void (*callback)(void);
    u16 config[4];
    s32 kind;
    u8 enabled;
    u8 unknown11[3];
    u32 argument;
};
struct ActorAbilitySourceInfo {
    u8 values[8];
    u32 argument;
};
#endif
