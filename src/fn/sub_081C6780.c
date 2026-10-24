#include "global.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
struct Position {
    s16 x, y, z, pad;
};
struct Command {
    u8 pad0[6];
    u8 kind;
    u8 pad7;
    union {
        struct Position position;
        u8 direction;
    } data;
};
struct Receiver {
    u8 pad0[6];
    u8 counts[2];
    struct Command *commands[2][4];
};
struct StateActor {
    u8 pad0[2], state, phase, behavior, pad5, kind, pad7, active, pending;
    u8 padA[6];
    u32 counter, timer;
    u8 flags18, facing4, facing8, angle;
    u8 pad1C[0x54 - 0x1C];
    struct Position target;
    u8 pad5C[0x68 - 0x5C];
    struct Position position;
    u8 pad70[0x168 - 0x70];
    struct Receiver receiver;
    u8 pad190[0x2CC - 0x190];
    void (*update)(void *);
};
extern u8 *gUnk_0200047C;
extern u16 gUnk_03005260[];
extern const u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
s32 Math_Atan2(s32, s32);
void SceneMessages_SetPendingTrigger(void *, void *);
void sub_0802EA90(void *);
static inline s32 RandomPart(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return (gUnk_0203B400[gUnk_03005308] >> 4) & 1023;
}
static inline void ResetMotion(struct StateActor *actor, u32 behavior)
{
    u8 zeroByte = 0;
    u32 zero = 0;
    actor->state = zeroByte;
    actor->phase = zeroByte;
    actor->behavior = behavior;
    actor->timer = zero;
    actor->active = 1;
}
// confidence=low: consumes facing/position messages and an input-triggered random target.
#define StateActor_ProcessDirectionMessages sub_081C6780
void StateActor_ProcessDirectionMessages(struct StateActor *actor)
{
    struct Receiver *receiver = &actor->receiver;
    s32 i;
    u32 k1;
    for (i = 0; i < receiver->counts[gUnk_0200047C[0x24]]; i++) {
        struct Command *command = receiver->commands[gUnk_0200047C[0x24]][i];
        switch (command->kind) {
            case 4:
                actor->target.x = command->data.position.x;
                actor->target.y = command->data.position.y;
                actor->target.z = command->data.position.z;
                actor->angle = Math_Atan2(actor->target.x - actor->position.x,
                                          actor->target.z - actor->position.z);
                ResetMotion(actor, 2);
                break;

            case 3:
                actor->angle = (command->data.direction + 5) << 5;
                ResetMotion(actor, 3);
                break;
        }

        SceneMessages_SetPendingTrigger(receiver, command);
    }

    if (gUnk_03005260[1] & 2) {
        s32 x = RandomPart();
        s32 z = RandomPart();
        struct Position *target = &actor->target;
        {
            typeof(target->x) *q1 = &target->x;
            u32 z1 = 256;
            *q1 = x + (k1 = 1024);
            target->y = z1;
            target->z = z + k1;
            actor->angle = Math_Atan2(target->x - actor->position.x, target->z - actor->position.z);
            ResetMotion(actor, 2);
            actor->kind = 3;
            actor->update = sub_0802EA90;
            actor->counter = 0;
            actor->pending = 1;
            actor->flags18 = 0;
        }
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081C6780);
#endif
