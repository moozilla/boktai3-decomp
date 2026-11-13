#ifndef GUARD_ACTOR_STAT_CONFIG_H
#define GUARD_ACTOR_STAT_CONFIG_H
#include "actor_stats.h"
/* Tentative compact parameter list; the leading descriptor words are not used here. */
struct ActorStatChoices {
    u32 descriptor[2];
    u8 values[4];
    u8 count;
};
#endif
