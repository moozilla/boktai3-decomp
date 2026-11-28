#ifndef GUARD_DELAYED_SCRIPT_H
#define GUARD_DELAYED_SCRIPT_H

#include "global.h"
#include "engine/task.h"

#define DELAYED_SCRIPT_CALL   1 /* run the script as a procedure, not a block */
#define DELAYED_SCRIPT_REPEAT 2

struct DelayedScriptArguments {
    u16 count;
    u32 *values;
};

/* A task that runs a script after a delay, or once a message arrives for its owner. */
struct DelayedScript {
    struct Task task;
    u32 owner;                          /* 0x18: message queue id; a 0xFFFF message cancels */
    struct DelayedScriptArguments args; /* 0x1C */
    u32 script;                         /* 0x24 */
    s32 timer;                          /* 0x28: frames left */
    u16 everyFrame;                     /* 0x2C: set when the delay was negative */
    u16 flags;                          /* 0x2E */
    u32 values[8];                      /* 0x30: copied arguments */
    struct QueueMessage *messages;      /* 0x50 */
};

typedef char DelayedScriptSizeCheck[sizeof(struct DelayedScript) == 0x54 ? 1 : -1];

#endif
