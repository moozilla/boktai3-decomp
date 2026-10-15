#ifndef GUARD_ACTOR_ATTACHMENT_H
#define GUARD_ACTOR_ATTACHMENT_H
#include "global.h"
/* Tentative screen-space attachment offsets, selected by kind/action/facing. */
void ActorAttachment_GetAction4Offset(s32, s32 *, s32 *, s32);
void ActorAttachment_GetAction1Offset(s32, s32 *, s32 *, s32);
void ActorAttachment_GetAction3Offset(s32, s32 *, s32 *, s32);
void ActorAttachment_GetActionGroupOffset(s32, s32 *, s32 *, s32);
void ActorAttachment_GetAction21Offset(s32, s32 *, s32 *, s32);
void sub_08064B20(s32, s32 *, s32 *, s32);
void ActorAttachment_GetAction25Offset(u32, s32 *, s32 *, s32);
void ActorAttachment_GetDefaultOffset(u32, s32 *, s32 *, s32);
void ActorAttachment_GetActionOffset(s32, u32, u32, s32 *, s32 *, void *);
#endif
