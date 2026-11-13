#ifndef GUARD_BESTIARY_H
#define GUARD_BESTIARY_H
#include "monster_book.h"
extern u32 gUnk_08613580[], gUnk_086135B8[], gUnk_086135F0[];
s32 Bestiary_GetCategoryFirstEntry(u32 i);
s32 Bestiary_GetCategoryEntryCount(u32 i);
s32 Bestiary_GetGroupEntryCount(s32 i);
s32 Bestiary_GetGroupFirstEntry(s32 n);
void Bestiary_SelectCategoryForEntry(struct MonsterBook *p);
s32 Bestiary_GetEntryGroup(s32 position);
s32 Bestiary_GetEntryForActor(u32 kind, s32 variant);
void Bestiary_UnlockReward(s32 a, s32 b);
void Bestiary_UnlockRewardFromScript(void);
void Bestiary_UnlockEntry(u32 flag);
void Bestiary_UnlockEntryFromScript(void);
u32 Bestiary_IsEntryUnlocked(s32 idx);
u32 Bestiary_IsRewardUnlocked(u32 a, u32 b);
u32 Bestiary_IsGroupComplete(s32 a);
u32 Bestiary_CountUnlockedGroupEntries(s32 a);
u32 Bestiary_AreAllEntriesUnlocked(void);
void Script_ConsumeOptionalKeywordValue(void);
void Bestiary_UpdateCompletionBadges(struct MonsterBook *p);
#endif
