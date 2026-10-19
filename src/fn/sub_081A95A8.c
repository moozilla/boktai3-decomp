
#ifdef NONMATCHING
// fuzzy: 95.3%
// foundry draft (worker best 95.3%); what differs is mostly register allocation and instruction order
#define Vanargand_UpdatePairedAttack sub_081A95A8
#define PairedSceneActor_IsFinished PairedSceneActor_AreSelectedHalvesFinished
#define gPlayer gUnk_02000580
#include "global.h"
struct Vanargand {
 u8 pad0[0x9C]; u32 flags;
 u8 padA0[2]; u8 phase; u8 padA3; u32 phaseTimer;
 u8 padA8[7]; u8 finished; u8 entered,pendingAnimation;
 u8 padB2[0x104-0xB2]; void *callback;
 u8 pad108[0x56C-0x108]; void *attachment; u8 pad570[8]; u32 resetTimer;
};
extern void *gUnk_02000580;
s32 Callback_InvokeActor(void *,void *);
void PairedSceneActor_ApplyToSelectedHalves(void *,s32,s32);
s32 Player_RequestTimedReset(void *,u32);
s32 ScenePair_CountPositiveCounters(void *);
void Sound_PlaySong(u32);
void Vanargand_SetPendingCue(void *,u16);
s32 PairedSceneActor_AreSelectedHalvesFinished(void *);
void sub_081A95A8(struct Vanargand *p)
{
 u8 result;
 u32 phase;
 u32 *timer;
 void *attachment;
 s32 cue,mode;
 u32 flags=0x40;
 p->flags |= flags;
 Vanargand_SetPendingCue(p,1);
 result=Callback_InvokeActor(p,p->callback);
 if(result) return;
 if(p->pendingAnimation) {
  p->pendingAnimation=result;
  p->phase=result;
  p->phaseTimer=result;
 }
 phase=p->phase;
 switch((s32)(u8)phase) {
 case 0:
  timer=&p->phaseTimer;
  if(*timer==0) {
   PairedSceneActor_ApplyToSelectedHalves(p->attachment,3,7);
   (*timer)++;
  } else if(PairedSceneActor_IsFinished(p->attachment)) {
   p->phase=1;
   *timer=phase;
  }
  break;
 case 1:
  timer=&p->phaseTimer;
  if(*timer==0) {
   PairedSceneActor_ApplyToSelectedHalves(p->attachment,3,16);
   (*timer)++;
  } else if(PairedSceneActor_IsFinished(p->attachment)) {
   s32 count=ScenePair_CountPositiveCounters(p->attachment);
   if(count>0) {
    void *player=gPlayer;
    u32 duration=count*72;
    Player_RequestTimedReset(player,duration);
    p->resetTimer=duration;
   }
   Sound_PlaySong(0x1BA);
   p->phase=2;
   *timer=result;
  }
  break;
 case 2:
  p->phaseTimer++;
  if(p->phaseTimer>60) {
   p->phase=3;
   p->phaseTimer=result;
  }
  break;
 case 3:
  p->finished=1;
  break;
 }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_081A95A8);
#endif
