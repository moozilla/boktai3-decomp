// MATCHING_CONCESSION: register-pinning
#include "global.h"
#define FIELD(p, n, t) (*(t *)((u8 *)(p) + (n)))
struct Slot7Command {
    u8 unknown0[6];
    u8 command, unknown7;
    s16 args[3];
};
struct Slot7Receiver {
    u8 unknown0[6];
    u8 count[2];
    struct Slot7Command *commands[2][4];
};
struct Slot7CommandOwner {
    u8 unknown0[0xAC];
    u8 state, substate, phase, complete, entered;
    u8 unknownB1[0xC4 - 0xB1];
    u32 counter;
    u8 unknownC8[0x118 - 0xC8];
    struct Slot7Receiver receiver;
};
struct Slot7CommandScene {
    u8 unknown0[0x24];
    u8 bank;
};
extern struct Slot7CommandScene *gSceneEventDispatchState;
extern u16 gUnk_03005040[];
void SceneMessages_SetPendingTrigger(void *, void *);
u32 Trigger_ConsumePending(void *, u32);
void Sound_PlaySong(u32);
void CpuSet(const void *, void *, u32);
void SingletonSlot7_SetMotionMode(void *, u32);
#ifdef NONMATCHING
// fuzzy: 44.6%
/* Loop setup, duplicated resource paths and state/control lifetimes still differ. */
static inline void SetPhase(struct Slot7CommandOwner *owner, u32 phase)
{
    u8 zero = 0;
    u32 wordZero = 0;
    owner->state = 1;
    owner->substate = zero;
    owner->phase = phase;
    owner->counter = wordZero;
    owner->entered = 1;
}
struct Slot7ResourceRows {
    u8 *palettes[4];
    u8 *resources[4];
};
static inline void SetResources(u8 *actor, u32 a, u32 b, u32 base)
{
    s32 i;
    u8 *dest;
    const u8 *source;
    struct Slot7ResourceRows *rows;
    u8 *resource;
    FIELD(actor, 0x5F4, u32) = FIELD(actor, 0x5FC, u32);
    FIELD(actor, 0x5F8, u32) = a;
    FIELD(actor, 0x600, u32) = FIELD(actor, 0x608, u32);
    FIELD(actor, 0x604, u32) = b;
    dest = actor + 0x508;
    source = (const u8 *)gUnk_03005040;
    i = 3;
    do {
        CpuSet(source, dest, 0x10);
        dest += 0x20;
        source += 0x20;
    } while (--i >= 0);
    asm("" : "+r"(actor));
    i = 0;
    dest = actor + 0x508;
    rows = (void *)(actor + 0x60C);
    resource = actor + base;
    do {
        rows->palettes[i] = dest;
        rows->resources[i] = resource;
        dest += 0x20;
        resource += 0x20;
    } while (++i <= 3);
}
// confidence=low: dispatches scene commands to the slot-seven motion and resource controls.
#define SingletonSlot7_DispatchPendingCommands sub_081F0734
void SingletonSlot7_DispatchPendingCommands(struct Slot7CommandOwner *input)
{
    register struct Slot7CommandOwner *owner asm("r8") = input;
    u8 *actor = (u8 *)owner;
    struct Slot7Receiver *receiver = &owner->receiver;
    s32 i = 0;
    if (i < receiver->count[gSceneEventDispatchState->bank])
        do {
            struct Slot7Command *command = receiver->commands[gSceneEventDispatchState->bank][i];
            register s32 next asm("r9");
            u32 kind;
            SceneMessages_SetPendingTrigger(receiver, command);
            kind = command->command;
            next = i + 1;
            asm("" : : "r"(next), "r"(owner), "r"(receiver));
            switch (kind) {
                case 0:
                    FIELD(actor, 0x294, s32) = -100;
                    FIELD(actor, 0x5D2, u16) = 1;
                    SetPhase(owner, 2);
                    break;
                case 1:
                    FIELD(actor, 0x294, s32) = command->args[0];
                    FIELD(actor, 0x5D2, u16) = 1;
                    SetPhase(owner, 3);
                    break;
                case 10:
                    FIELD(actor, 0x5C2, u16) = 15;
                    Sound_PlaySong(0x587);
                    Trigger_ConsumePending(receiver, 1);
                    break;
                case 58:
                    FIELD(actor, 0x294, s32) = command->args[0];
                    FIELD(actor, 0x5D2, u16) = command->args[1];
                    SetPhase(owner, 4);
                    break;
                case 59:
                    SetPhase(owner, 7);
                    break;
                case 60:
                    FIELD(actor, 0x5A5, u8) = command->args[0];
                    Trigger_ConsumePending(receiver, 1);
                    break;
                case 61:
                    if (command->args[0] == 0) {
                        FIELD(actor, 0x5A0, u32) = 0;
                        FIELD(actor, 0x304, u32) = 0;
                        FIELD(actor, 0x34C, u32) = 0;
                        Trigger_ConsumePending(receiver, 1);
                    } else {
                        FIELD(actor, 0x5A6, u16) = 2;
                        SetPhase(owner, 9);
                    }
                    break;
                case 62:
                    SetPhase(owner, 8);
                    break;
                case 16:
                    if (command->args[0] == 0) {
                        SetResources(actor, 3, 2, 0x408);
                    } else if (command->args[1] == 0) {
                        SetResources(actor, 502, 501, 0x488);
                    } else {
                        SetResources(actor, 502, 501, 0x488);
                    }
                    FIELD(actor, 0x5FC, u32) = FIELD(actor, 0x5F8, u32);
                    FIELD(actor, 0x608, u32) = FIELD(actor, 0x604, u32);
                    FIELD(actor, 0x5A8, u32) = 0;
                    Trigger_ConsumePending(receiver, 1);
                    break;
                case 64:
                    FIELD(actor, 0x5D8, u16) = command->args[0];
                    FIELD(actor, 0x5DA, u16) = command->args[1];
                    FIELD(actor, 0x5DC, u16) = command->args[2];
                    FIELD(actor, 0x5A6, u16) = 0;
                    Trigger_ConsumePending(receiver, 1);
                    break;
                case 2:
                    FIELD(actor, 0x278, u16) = command->args[0];
                    FIELD(actor, 0x294, s32) = FIELD(actor, 0x278, s16);
                    FIELD(actor, 0x5D2, u16) = 1;
                    Trigger_ConsumePending(receiver, 1);
                    break;
                case 63:
                    if (command->args[0] == 0) {
                        SetResources(actor, 3, 2, 0x408);
                    } else {
                        SetResources(actor, 502, 501, 0x488);
                    }
                    FIELD(actor, 0x5FC, u32) = FIELD(actor, 0x5F8, u32);
                    FIELD(actor, 0x608, u32) = FIELD(actor, 0x604, u32);
                    FIELD(actor, 0x5A8, u32) = 0;
                    Sound_PlaySong(0x598);
                    Trigger_ConsumePending(receiver, 1);
                    break;
                case 65:
                    SingletonSlot7_SetMotionMode(actor, 0);
                    Trigger_ConsumePending(receiver, 1);
                    break;
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 17:
                case 18:
                case 19:
                case 20:
                case 21:
                case 22:
                case 23:
                case 24:
                case 25:
                case 26:
                case 27:
                case 28:
                case 29:
                case 30:
                case 31:
                case 32:
                case 33:
                case 34:
                case 35:
                case 36:
                case 37:
                case 38:
                case 39:
                case 40:
                case 41:
                case 42:
                case 43:
                case 44:
                case 45:
                case 46:
                case 47:
                case 48:
                case 49:
                case 50:
                case 51:
                case 52:
                case 53:
                case 54:
                case 55:
                case 56:
                case 57:
                default:
                    break;
            }
            i = next;
        } while (i < receiver->count[gSceneEventDispatchState->bank]);
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081F0734);
#endif
