#include "global.h"
#include "directional_interaction.h"
struct ResultMessage {
    u8 unk0[0x20];
    void (*callback)(void);
    const u8 *successScript, *failureScript;
    u32 unk2c;
    s32 window;
    u8 state, pending;
    u8 unk36[2];
    u32 result;
    u8 unk3c[12];
    u32 script;
};
void ResultMessage_ShowOutcome(struct ResultMessage *, u32, u32, u32);
void TimedMessage_ShowNotice(void);
void ResultMessage_RunOutcomeScript(struct ResultMessage *obj, u32 arg1, u32 arg2, u32 result)
{
    s32 value;
    struct DirectionalScriptArgs args;
    if (result == 0) {
        ResultMessage_ShowOutcome(obj, arg1, 0, 0);
    } else {
        obj->result = result;
        if (result <= 31)
            *(u32 *)gGameState->unk820 |= 1 << result;
        if (obj->script) {
            value = obj->result;
            args.count = 1;
            args.values = &value;
            Script_Call(obj->script, &args);
            {
                void (*callback)(void) = TimedMessage_ShowNotice;
                u32 state = 5;
                obj->callback = callback;
                obj->pending = 1;
                obj->state = state;
            }
        }
    }
}
