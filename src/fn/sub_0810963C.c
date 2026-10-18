#include "global.h"
#include "directional_interaction.h"
#define ResultMessage_RunOutcomeScript sub_0810963C
#define ResultMessage_ShowOutcome sub_081095B4
struct ResultMessage {
 u8 unk0[0x20]; void (*callback)(void); const u8 *successScript, *failureScript;
 u32 unk2c; s32 window; u8 state, pending; u8 unk36[2]; u32 result;
 u8 unk3c[12]; u32 script;
};
void sub_081095B4(struct ResultMessage *,u32,u32,u32);
void sub_081096CC(void);
void sub_0810963C(struct ResultMessage *obj, u32 arg1, u32 arg2, u32 result)
{
 s32 value;
 struct DirectionalScriptArgs args;
 if (result == 0) {
  sub_081095B4(obj,arg1,0,0);
 } else {
  obj->result = result;
  if (result <= 31) *(u32 *)gUnk_02000710->unk820 |= 1 << result;
  if (obj->script) {
   value = obj->result;
   args.count = 1;
   args.values = &value;
   Script_Call(obj->script,&args);
   {
    void (*callback)(void) = sub_081096CC;
    u32 state = 5;
    obj->callback = callback;
    obj->pending = 1;
    obj->state = state;
   }
  }
 }
}
