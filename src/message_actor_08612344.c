#include "state_actor.h"

/* Scripted actor kind with state table gUnk_08612344: states 0x0E-0x49 are
 * command states that step through a scene-message sequence. */

extern u32 gUnk_08612344[];
void sub_08249240(void *, struct StateActor *, u32); /* _call_via_r2 */
void sub_0802EA08(void);
void SceneMessages_RegisterReceiver(u8 *, u32, u32);
void sub_081CD43C(void *ctx, struct StateActor *p);
void sub_081CD454(void *ctx, struct StateActor *p);

void sub_081CC690(void *ctx, struct StateActor *p)
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
void sub_081CC6CC(void *ctx, struct StateActor *p)
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

STATE_COMMAND_GO_TIMED(sub_081CC700, 0x14)
STATE_COMMAND_WAIT_TIMED(sub_081CC72C, 0x15)
STATE_COMMAND_WAIT_TIMED(sub_081CC760, 0x16)
STATE_COMMAND_GO_TIMED(sub_081CC794, 0x17)
STATE_COMMAND_GO_TIMED(sub_081CC7C0, 0x18)
STATE_COMMAND_GO_TIMED(sub_081CC7EC, 0x19)
STATE_COMMAND_GO_TIMED(sub_081CC818, 0x1a)
STATE_COMMAND_WAIT(sub_081CC844, 0x1b)
STATE_COMMAND_WAIT(sub_081CC86C, 0x1c)
STATE_COMMAND_WAIT(sub_081CC894, 0x1d)
STATE_COMMAND_GO(sub_081CC8BC, 0x1f)
STATE_COMMAND_WAIT(sub_081CC8E0, 0x20)
STATE_COMMAND_WAIT(sub_081CC908, 0x21)
STATE_COMMAND_RESTART(sub_081CC930, 0x22)
STATE_COMMAND_WAIT(sub_081CC964, 0x23)
STATE_COMMAND_RESTART(sub_081CC98C, 0x24)
STATE_COMMAND_WAIT(sub_081CC9C0, 0x25)
STATE_COMMAND_WAIT(sub_081CC9E8, 0x26)
STATE_COMMAND_RESTART(sub_081CCA10, 0x27)
STATE_COMMAND_GO(sub_081CCA44, 0x28)
STATE_COMMAND_WAIT(sub_081CCA68, 0x29)
void sub_081CCA90(void *ctx, struct StateActor *p)
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

STATE_COMMAND_GO(sub_081CCAC8, 0x2b)
STATE_COMMAND_RESTART(sub_081CCAEC, 0x2c)
STATE_COMMAND_WAIT(sub_081CCB20, 0x2d)
STATE_COMMAND_WAIT(sub_081CCB48, 0x2e)
STATE_COMMAND_WAIT(sub_081CCB70, 0x2f)
STATE_COMMAND_WAIT(sub_081CCB98, 0x30)
STATE_COMMAND_WAIT(sub_081CCBC0, 0x31)
STATE_COMMAND_WAIT(sub_081CCBE8, 0x32)
STATE_COMMAND_WAIT(sub_081CCC10, 0x33)
STATE_COMMAND_WAIT(sub_081CCC38, 0x34)
STATE_COMMAND_WAIT(sub_081CCC60, 0x35)
STATE_COMMAND_WAIT(sub_081CCC88, 0x36)
STATE_COMMAND_WAIT(sub_081CCCB0, 0x37)
STATE_COMMAND_WAIT(sub_081CCCD8, 0x38)
STATE_COMMAND_GO(sub_081CCD00, 0x39)
STATE_COMMAND_WAIT(sub_081CCD24, 0x3a)
STATE_COMMAND_WAIT(sub_081CCD4C, 0x3b)
STATE_COMMAND_REPEAT(sub_081CCD74, 0x3c, 0x3d)
STATE_COMMAND_REPEAT(sub_081CCDA8, 0x3c, 0x3e)
STATE_COMMAND_WAIT(sub_081CCDDC, 0x3c)
STATE_COMMAND_WAIT(sub_081CCE04, 0x3d)
STATE_COMMAND_WAIT(sub_081CCE2C, 0x3e)
STATE_COMMAND_WAIT(sub_081CCE54, 0x3f)
STATE_COMMAND_WAIT(sub_081CCE7C, 0x40)
STATE_COMMAND_WAIT(sub_081CCEA4, 0x41)
STATE_COMMAND_WAIT(sub_081CCECC, 0x42)
STATE_COMMAND_GO(sub_081CCEF4, 0x43)
STATE_COMMAND_WAIT(sub_081CCF18, 0x44)
STATE_COMMAND_GO(sub_081CCF40, 0x45)
STATE_COMMAND_WAIT(sub_081CCF64, 0x46)
STATE_COMMAND_GO(sub_081CCF8C, 0x47)
STATE_COMMAND_WAIT(sub_081CCFB0, 0x48)
/* Last command: hand the actor back to its idle callback. */
void sub_081CCFD8(void *ctx, struct StateActor *p)
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
            void (*next)(void) = sub_0802EA08;
            p->prevState = one;
            p->stateCallback = next;
        }
        p->stateFrame = zero;
        p->enter = one;
        p->phase = zero;
    }
}

STATE_COMMAND_WAIT(sub_081CD024, 0x4a)
STATE_COMMAND_GO(sub_081CD04C, 0x4b)
STATE_COMMAND_WAIT(sub_081CD070, 0x4c)
STATE_COMMAND_WAIT(sub_081CD098, 0x4d)
STATE_COMMAND_WAIT(sub_081CD0C0, 0x4e)
STATE_COMMAND_GO(sub_081CD0E8, 0x4f)
STATE_COMMAND_WAIT(sub_081CD10C, 0x50)
STATE_COMMAND_WAIT(sub_081CD134, 0x51)
STATE_COMMAND_WAIT(sub_081CD15C, 0x52)
STATE_COMMAND_WAIT(sub_081CD184, 0x53)
STATE_COMMAND_WAIT(sub_081CD1AC, 0x54)
STATE_COMMAND_WAIT(sub_081CD1D4, 0x55)
STATE_COMMAND_WAIT(sub_081CD1FC, 0x56)
STATE_COMMAND_WAIT(sub_081CD224, 0x57)
STATE_COMMAND_WAIT(sub_081CD24C, 0x58)
STATE_COMMAND_WAIT(sub_081CD274, 0x59)
STATE_COMMAND_WAIT(sub_081CD29C, 0x5a)
STATE_COMMAND_WAIT(sub_081CD2C4, 0x5b)
STATE_COMMAND_WAIT(sub_081CD2EC, 0x5c)
STATE_COMMAND_WAIT(sub_081CD314, 0x5d)
STATE_COMMAND_WAIT(sub_081CD33C, 0x5e)
STATE_COMMAND_GO(sub_081CD364, 0x5f)
STATE_COMMAND_GO(sub_081CD388, 0x0f)
STATE_COMMAND_GO(sub_081CD3AC, 0x10)
STATE_COMMAND_GO(sub_081CD3D0, 0x11)
STATE_COMMAND_GO(sub_081CD3F4, 0x12)
STATE_COMMAND_GO(sub_081CD418, 0x13)
void sub_081CD43C(void *ctx, struct StateActor *p)
{
    sub_08249240(ctx, p, gUnk_08612344[p->state]);
}

void sub_081CD454(void *ctx, struct StateActor *p)
{
    sub_081CD43C(ctx, p);
}

s32 sub_081CD460(struct StateActor *p)
{
    /* called with the actor only; ctx is left in r1 */
    ((void (*)(struct StateActor *))sub_081CD454)(p);
    return 0;
}

void sub_081CD46C(struct StateActor *p, u32 id)
{
    u32 buf[4];
    SceneMessages_RegisterReceiver(p->receiver, id, 5);
}
