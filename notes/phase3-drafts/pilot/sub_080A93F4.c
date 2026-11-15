// Holdout draft (no concessions), 57 rows: shared PlayerCue_StepSequence step, row-indexed loop, speed read through a spilled copy of body.
#include "display_animation.h"
#include "global.h"
#include "player_cue_sequence.h"
#include "tentacle_segment_animation.h"

void *SceneActorProfiles_GetEntryAddress(u32, u32, s32);
void TentacleBody_GetDirectionFrame(u16, s32 *, s32 *);
void Anim_RestartSequence(void *, void *, u32, u8, u8);
void Anim_SetDurationScale(void *, u32);
static inline u8 TestMask(struct TentacleSegmentAnimation *p, u32 mask) {
  if (p->status & mask)
    return 1;
  return 0;
}
void TentacleBody_RestartSegmentAnimations(
    struct TentacleBodyAnimationView *body, s32 kind, s32 row, s32 flags) {
  struct TentacleBodyAnimationView *actor = body;
  u8 *profile =
      SceneActorProfiles_GetEntryAddress(body->profileId, body->profileMode, 0);
  s32 i;
  for (i = 0; i < body->segmentCount; i++) {
    struct TentacleSegmentAnimation *a = &body->segments[i].animation;
    if (TestMask(a, 1))
      continue;
    if (TentacleSegment_HasStatus(a, 4))
      Anim_RestartSequence(
          a, TentacleSegment_ScriptAt(body, i * sizeof(body->segments[0])), 1,
          row, flags);
    else {
      Anim_RestartSequence(
          a, TentacleSegment_ScriptAt(body, i * sizeof(body->segments[0])),
          profile[kind * 4], row, flags);
      Anim_SetDurationScale(a, actor->speed);
    }
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&body->segments[i],
                           &a->sequence);
  }
}
