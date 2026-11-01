#include "scene_departure_display.h"

extern u16 gUnk_03005040[4][16];
void SingletonSlot7_SetMotionMode(void *, u32);

struct PendingCommand {
    u8 unknown0[6];
    u8 kind, unknown7;
    s16 args[3];
};

#define TRANSITION_PALETTE_BANKS(owner, fgBank, nextAccentBank, target)                            \
    {                                                                                              \
        s32 i;                                                                                     \
        (owner)->foregroundFrom = (owner)->foregroundBank;                                         \
        (owner)->foregroundTo = (fgBank);                                                          \
        (owner)->accentFrom = (owner)->accentBank;                                                 \
        (owner)->accentTo = (nextAccentBank);                                                      \
        for (i = 0; i < 4; i++)                                                                    \
            CpuSet(gUnk_03005040[i], (owner)->savedPalettes[i], 16);                               \
        {                                                                                          \
            u16 *source, **row, *dest;                                                             \
            i = 0;                                                                                 \
            source = (owner)->savedPalettes[0];                                                    \
            row = (owner)->paletteLinks.rows;                                                      \
            dest = (owner)->target[0];                                                             \
            do {                                                                                   \
                row[0] = source;                                                                   \
                row[4] = dest;                                                                     \
                source += 16;                                                                      \
                row++;                                                                             \
                dest += 16;                                                                        \
                if (++i > 3)                                                                       \
                    break;                                                                         \
            } while (1);                                                                           \
        }                                                                                          \
    }

void SingletonSlot7_DispatchPendingCommands(struct DepartureActor *owner)
{
    struct DepartureActor *actor = owner;
    struct SceneEventReceiver *receiver = &owner->messages;
    s32 i = 0;
    if (i < receiver->count[gSceneEventDispatchState->phase]) {
        do {
            struct SceneEventDispatchState *scene = gSceneEventDispatchState;
            struct PendingCommand *command = (void *)receiver->pending[scene->phase][i];
            u32 kind;
            SceneMessages_SetPendingTrigger(receiver, (void *)command);
            kind = command->kind;

            switch (kind) {
                case 0:
                    actor->targetX = -100;
                    actor->targetMode = 1;
                    DepartureActor_StartPhase(owner, 1, 0, 2);
                    break;
                case 1:
                    actor->targetX = command->args[0];
                    actor->targetMode = 1;
                    DepartureActor_StartPhase(owner, 1, 0, 3);
                    break;
                case 10:
                    actor->slowdownTimer = 15;
                    Sound_PlaySong(0x587);
                    Trigger_ConsumePending(&owner->messages, 1);
                    break;
                case 58:
                    actor->targetX = command->args[0];
                    actor->targetMode = command->args[1];
                    DepartureActor_StartPhase(owner, 1, 0, 4);
                    break;
                case 59:
                    DepartureActor_StartPhase(owner, 1, 0, 7);
                    break;
                case 60:
                    actor->effectEnabled = command->args[0];
                    Trigger_ConsumePending(&owner->messages, 1);
                    break;
                case 61:
                    if (command->args[0] == 0) {
                        actor->dispersalTimer = 0;
                        actor->foreground.mode = 0;
                        actor->accent.mode = 0;
                        Trigger_ConsumePending(&owner->messages, 1);
                    } else {
                        actor->effectStyle = 2;
                        DepartureActor_StartPhase(owner, 1, 0, 9);
                    }
                    break;
                case 62:
                    DepartureActor_StartPhase(owner, 1, 0, 8);
                    break;
                case 16:
                    if (command->args[0] == 0) {
                        TRANSITION_PALETTE_BANKS(actor, 3, 2, fadeFrom);
                    } else if (command->args[1] == 0) {
                        TRANSITION_PALETTE_BANKS(actor, 502, 501, fadeTo);
                    } else {
                        TRANSITION_PALETTE_BANKS(actor, 502, 501, fadeTo);
                    }
                    actor->foregroundBank = actor->foregroundTo;
                    actor->accentBank = actor->accentTo;
                    actor->fadeFrame = 0;
                    Trigger_ConsumePending(&owner->messages, 1);
                    break;
                case 64:
                    actor->burstX = command->args[0];
                    actor->burstY = command->args[1];
                    actor->burstTimer = command->args[2];
                    actor->effectStyle = 0;
                    Trigger_ConsumePending(&owner->messages, 1);
                    break;
                case 2:
                    actor->screen.x = command->args[0];
                    actor->targetX = actor->screen.x;
                    actor->targetMode = 1;
                    Trigger_ConsumePending(&owner->messages, 1);
                    break;
                case 63:
                    if (command->args[0] == 0) {
                        TRANSITION_PALETTE_BANKS(actor, 3, 2, fadeFrom);
                    } else {
                        TRANSITION_PALETTE_BANKS(actor, 502, 501, fadeTo);
                    }
                    actor->foregroundBank = actor->foregroundTo;
                    actor->accentBank = actor->accentTo;
                    actor->fadeFrame = 0;
                    Sound_PlaySong(0x598);
                    Trigger_ConsumePending(&owner->messages, 1);
                    break;
                case 65:
                    SingletonSlot7_SetMotionMode(actor, 0);
                    Trigger_ConsumePending(&owner->messages, 1);
                    break;
            }
            i++;
            if (i >= receiver->count[gSceneEventDispatchState->phase])
                break;
        } while (1);
    }
}
