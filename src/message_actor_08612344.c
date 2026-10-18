#include "state_actor.h"

/* Scripted actor kind with state table gUnk_08612344: states 0x0E-0x49 are
 * command states that step through a scene-message sequence. */

extern u32 gUnk_08612344[];
void sub_08249240(void *, struct StateActor *, u32); /* _call_via_r2 */
void Actor_OnHiddenIdleUpdate(void);
void SceneMessages_RegisterReceiver(u8 *, u32, u32);
void SceneCharacter_DispatchCommandState(void *ctx, struct StateActor *p);
void SceneCharacter_UpdateCommands(void *ctx, struct StateActor *p);

void SceneCharacter_WaitForState0EAndResume(void *ctx, struct StateActor *p)
{
    u32 one;
    u32 zero;
    if (p->enterCommand != 0) {
        p->enterCommand = 0;
        p->done = 0;
        p->nextState = 14;
    }
    if (p->done != 0) {
        one = 1;
        zero = 0;
        p->state = one;
        p->timer = zero;
        p->enterCommand = one;
        Trigger_ConsumePending(p->receiver, 1);
    }
    p->timer += 1;
}

/* Command 0x0E: make the current position the walk target. */
void SceneCharacter_SetWalkTargetToCurrentPosition(void *ctx, struct StateActor *p)
{
    if (p->enterCommand) {
        p->enterCommand = 0;
        p->done = 0;
        p->target = p->position;
        p->nextState = 0xe;
        Trigger_ConsumePending(p->receiver, 1);
    }
    p->timer += 1;
}

STATE_COMMAND_GO_TIMED(SceneCharacter_StartTimedState14, 0x14)
STATE_COMMAND_WAIT_TIMED(SceneCharacter_WaitForTimedState15, 0x15)
STATE_COMMAND_WAIT_TIMED(SceneCharacter_WaitForTimedState16, 0x16)
STATE_COMMAND_GO_TIMED(SceneCharacter_StartTimedState17, 0x17)
STATE_COMMAND_GO_TIMED(SceneCharacter_StartTimedState18, 0x18)
STATE_COMMAND_GO_TIMED(SceneCharacter_StartTimedState19, 0x19)
STATE_COMMAND_GO_TIMED(SceneCharacter_StartTimedState1A, 0x1a)
STATE_COMMAND_WAIT(SceneCharacter_WaitForState1B, 0x1b)
STATE_COMMAND_WAIT(SceneCharacter_WaitForState1C, 0x1c)
STATE_COMMAND_WAIT(SceneCharacter_WaitForState1D, 0x1d)
STATE_COMMAND_GO(SceneCharacter_StartState1F, 0x1f)
STATE_COMMAND_WAIT(SceneCharacter_WaitForState20, 0x20)
STATE_COMMAND_WAIT(SceneCharacter_WaitForState21, 0x21)
STATE_COMMAND_RESTART(SceneCharacter_WaitForState22AndResume, 0x22)
STATE_COMMAND_WAIT(SceneCharacter_WaitForState23, 0x23)
STATE_COMMAND_RESTART(SceneCharacter_WaitForState24AndResume, 0x24)
STATE_COMMAND_WAIT(SceneCharacter_WaitForState25, 0x25)
STATE_COMMAND_WAIT(SceneCharacter_WaitForState26, 0x26)
STATE_COMMAND_RESTART(MessageActor_OnState27ReturnToIdle, 0x27)
STATE_COMMAND_GO(MessageActor_OnState28AdvanceMessage, 0x28)
STATE_COMMAND_WAIT(MessageActor_OnState29WaitForCompletion, 0x29)
void MessageActor_OnState2AAdvanceAndWait(void *ctx, struct StateActor *p)
{
    if (p->enterCommand) {
        p->enterCommand = 0;
        p->done = 0;
        p->nextState = 0x2a;
        Trigger_ConsumePending(p->receiver, 1);
    }
    if (p->done)
        Trigger_ConsumePending(p->receiver, 1);
}

STATE_COMMAND_GO(MessageActor_OnState2BAdvanceMessage, 0x2b)
STATE_COMMAND_RESTART(MessageActor_OnState2CReturnToIdle, 0x2c)
STATE_COMMAND_WAIT(MessageActor_OnState2DWaitForCompletion, 0x2d)
STATE_COMMAND_WAIT(MessageActor_OnState2EWaitForCompletion, 0x2e)
STATE_COMMAND_WAIT(MessageActor_OnState2FWaitForCompletion, 0x2f)
STATE_COMMAND_WAIT(MessageActor_OnState30WaitForCompletion, 0x30)
STATE_COMMAND_WAIT(MessageActor_OnState31WaitForCompletion, 0x31)
STATE_COMMAND_WAIT(MessageActor_OnState32WaitForCompletion, 0x32)
STATE_COMMAND_WAIT(MessageActor_OnState33WaitForCompletion, 0x33)
STATE_COMMAND_WAIT(MessageActor_OnState34WaitForCompletion, 0x34)
STATE_COMMAND_WAIT(MessageActor_OnState35WaitForCompletion, 0x35)
STATE_COMMAND_WAIT(MessageActor_OnState36WaitForCompletion, 0x36)
STATE_COMMAND_WAIT(MessageActor_OnState37WaitForCompletion, 0x37)
STATE_COMMAND_WAIT(MessageActor_OnState38WaitForCompletion, 0x38)
STATE_COMMAND_GO(MessageActor_OnState39AdvanceMessage, 0x39)
STATE_COMMAND_WAIT(MessageActor_OnState3AWaitForCompletion, 0x3a)
STATE_COMMAND_WAIT(MessageActor_WaitForState3B, 0x3b)
STATE_COMMAND_REPEAT(MessageActor_RepeatState3CInto3D, 0x3c, 0x3d)
STATE_COMMAND_REPEAT(MessageActor_RepeatState3CInto3E, 0x3c, 0x3e)
STATE_COMMAND_WAIT(MessageActor_WaitForState3C, 0x3c)
STATE_COMMAND_WAIT(MessageActor_WaitForState3D, 0x3d)
STATE_COMMAND_WAIT(MessageActor_WaitForState3E, 0x3e)
STATE_COMMAND_WAIT(MessageActor_WaitForState3F, 0x3f)
STATE_COMMAND_WAIT(MessageActor_WaitForState40, 0x40)
STATE_COMMAND_WAIT(MessageActor_WaitForState41, 0x41)
STATE_COMMAND_WAIT(MessageActor_WaitForState42, 0x42)
STATE_COMMAND_GO(MessageActor_StartState43, 0x43)
STATE_COMMAND_WAIT(MessageActor_WaitForState44, 0x44)
STATE_COMMAND_GO(MessageActor_StartState45, 0x45)
STATE_COMMAND_WAIT(MessageActor_WaitForState46, 0x46)
STATE_COMMAND_GO(MessageActor_StartState47, 0x47)
STATE_COMMAND_WAIT(MessageActor_WaitForState48, 0x48)
/* Last command: hand the actor back to its idle callback. */
void MessageActor_RestoreIdleAfterState49(void *ctx, struct StateActor *p)
{
    if (p->enterCommand) {
        p->enterCommand = 0;
        p->done = 0;
        p->nextState = 0x49;
    }
    if (p->done) {
        u32 zero, one;
        Trigger_ConsumePending(p->receiver, 1);
        zero = 0;
        p->state = zero;
        p->timer = zero;
        one = 1;
        p->enterCommand = one;
        {
            void (*next)(void) = Actor_OnHiddenIdleUpdate;
            p->prevState = one;
            p->stateCallback = next;
        }
        p->stateFrame = zero;
        p->enter = one;
        p->phase = zero;
    }
}

STATE_COMMAND_WAIT(MessageActor_WaitForState4A, 0x4a)
STATE_COMMAND_GO(MessageActor_StartState4B, 0x4b)
STATE_COMMAND_WAIT(MessageActor_WaitForState4C, 0x4c)
STATE_COMMAND_WAIT(MessageActor_WaitForState4D, 0x4d)
STATE_COMMAND_WAIT(MessageActor_WaitForState4E, 0x4e)
STATE_COMMAND_GO(MessageActor_StartState4F, 0x4f)
STATE_COMMAND_WAIT(MessageActor_WaitForState50, 0x50)
STATE_COMMAND_WAIT(MessageActor_WaitForState51, 0x51)
STATE_COMMAND_WAIT(MessageActor_WaitForState52, 0x52)
STATE_COMMAND_WAIT(MessageActor_WaitForState53, 0x53)
STATE_COMMAND_WAIT(MessageActor_WaitForState54, 0x54)
STATE_COMMAND_WAIT(MessageActor_WaitForState55, 0x55)
STATE_COMMAND_WAIT(MessageActor_WaitForState56, 0x56)
STATE_COMMAND_WAIT(MessageActor_WaitForState57, 0x57)
STATE_COMMAND_WAIT(MessageActor_WaitForState58, 0x58)
STATE_COMMAND_WAIT(MessageActor_WaitForState59, 0x59)
STATE_COMMAND_WAIT(MessageActor_WaitForState5A, 0x5a)
STATE_COMMAND_WAIT(MessageActor_WaitForState5B, 0x5b)
STATE_COMMAND_WAIT(MessageActor_WaitForState5C, 0x5c)
STATE_COMMAND_WAIT(MessageActor_WaitForState5D, 0x5d)
STATE_COMMAND_WAIT(MessageActor_WaitForState5E, 0x5e)
STATE_COMMAND_GO(MessageActor_StartState5F, 0x5f)
STATE_COMMAND_GO(MessageActor_StartState0F, 0x0f)
STATE_COMMAND_GO(SceneCharacter_SelectCommandState10, 0x10)
STATE_COMMAND_GO(SceneCharacter_SelectCommandState11, 0x11)
STATE_COMMAND_GO(SceneCharacter_SelectCommandState12, 0x12)
STATE_COMMAND_GO(SceneCharacter_SelectCommandState13, 0x13)
void SceneCharacter_DispatchCommandState(void *ctx, struct StateActor *p)
{
    sub_08249240(ctx, p, gUnk_08612344[p->state]);
}

void SceneCharacter_UpdateCommands(void *ctx, struct StateActor *p)
{
    SceneCharacter_DispatchCommandState(ctx, p);
}

s32 SceneCharacter_OnCommandUpdate(struct StateActor *p)
{
    /* called with the actor only; ctx is left in r1 */
    ((void (*)(struct StateActor *))SceneCharacter_UpdateCommands)(p);
    return 0;
}

void SceneCharacter_RegisterMessageReceiver(struct StateActor *p, u32 id)
{
    u32 buf[4];
    SceneMessages_RegisterReceiver(p->receiver, id, 5);
}
