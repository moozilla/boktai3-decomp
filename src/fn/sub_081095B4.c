#include "global.h"
#define ResultMessage_ShowOutcome sub_081095B4
struct ResultMessage {
 u8 unk0[0x20]; void (*callback)(void); const u8 *successScript, *failureScript;
 u32 unk2c; s32 window; u8 state, pending;
};
void TextWindow_Clear(u32);
void TextWindow_StartPlayback(int);
s32 TextWindow_SetScript(s32, const u8 *);
s32 TextWindow_Refresh(s32,s32);
void Sound_PlaySong(u32);
void sub_0810990C(void);
void sub_081095B4(struct ResultMessage *obj, u32 unused, s32 success)
{
 if (success) {
  TextWindow_Clear(obj->window);
  TextWindow_SetScript(obj->window,obj->successScript);
  TextWindow_Refresh(obj->window,2);
  TextWindow_StartPlayback(obj->window);
  {
   void (*callback)(void) = sub_0810990C;
   u32 state = 2;
   obj->callback = callback;
   obj->pending = 1;
   obj->state = state;
  }
  Sound_PlaySong(0xde);
 } else {
  TextWindow_Clear(obj->window);
  TextWindow_SetScript(obj->window,obj->failureScript);
  TextWindow_Refresh(obj->window,3);
  TextWindow_StartPlayback(obj->window);
  {
   void (*callback)(void) = sub_0810990C;
   u32 state = 2;
   obj->callback = callback;
   obj->pending = 1;
   obj->state = state;
  }
  Sound_PlaySong(0x192);
 }
}
