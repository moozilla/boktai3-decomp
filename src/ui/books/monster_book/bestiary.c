#include "ui/books/bestiary.h"

s32 Bestiary_GetCategoryFirstEntry(u32 category)
{
    return gUnk_08613580[category];
}

s32 Bestiary_GetCategoryEntryCount(u32 category)
{
    return gUnk_086135B8[category];
}

s32 Bestiary_GetGroupEntryCount(s32 group)
{
    return gUnk_086135F0[group];
}

s32 Bestiary_GetGroupFirstEntry(s32 group)
{
    s32 firstEntry = 0;
    s32 previousGroup;
    for (previousGroup = 0; previousGroup < group; previousGroup++)
        firstEntry += Bestiary_GetGroupEntryCount(previousGroup);
    return firstEntry;
}

void Bestiary_SelectCategoryForEntry(struct MonsterBook *book)
{
    s32 i;
    for (i = 0; i <= 13; i++) {
        if (book->entry < Bestiary_GetCategoryFirstEntry(i)) {
            book->category = i - 1;
            book->selectedRow = book->entry - Bestiary_GetCategoryFirstEntry(book->category);
            return;
        }
    }
    book->category = 13;
    book->selectedRow = book->entry - Bestiary_GetCategoryFirstEntry(13);
}

s32 Bestiary_GetEntryGroup(s32 entry)
{
    if (entry < Bestiary_GetCategoryFirstEntry(3))
        return 0;
    if (entry < Bestiary_GetCategoryFirstEntry(5))
        return 1;
    if (entry < Bestiary_GetCategoryFirstEntry(11))
        return 2;
    if (entry < Bestiary_GetCategoryFirstEntry(13))
        return 3;
    return 4;
}

s32 Bestiary_GetEntryForActor(u32 kind, s32 variant)
{
    switch (kind) {
        case 1:
            switch (variant) {
                case 0:
                    return 23;
                case 1:
                    return 24;
                case 2:
                    return 25;
                default:
                    return -1;
            }
        case 2:
            switch (variant) {
                case 1:
                    return 5;
                case 0:
                case 2:
                    return 4;
                default:
                    return -1;
            }
        case 3:
            switch (variant) {
                case 0:
                    return 44;
                case 1:
                    return 45;
                case 3:
                    return 46;
                default:
                    return -1;
            }
        case 4:
            switch (variant) {
                case 1:
                    return 20;
                case 2:
                    return 21;
                case 3:
                    return 22;
                default:
                    return -1;
            }
        case 5:
            switch (variant) {
                case 0:
                    return 37;
                case 5:
                    return 43;
                case 12:
                    return 40;
                default:
                    return -1;
            }
        case 6:
            switch (variant) {
                case 0:
                    return 38;
                case 12:
                    return 41;
                default:
                    return -1;
            }
        case 7:
            switch (variant) {
                case 0:
                    return 39;
                case 12:
                    return 42;
                default:
                    return -1;
            }
        case 9:
            return 13;
        case 10:
            return 47;
        case 11:
            switch (variant) {
                case 0:
                    return 48;
                case 1:
                    return 49;
                default:
                    return -1;
            }
        case 12:
        case 13:
            switch (variant) {
                case 0:
                    return 50;
                case 1:
                    return 51;
                default:
                    return -1;
            }
        case 14:
            return 19;
        case 17:
            switch (variant) {
                case 0:
                    return 68;
                case 1:
                    return 69;
                case 2:
                    return 70;
                default:
                    return -1;
            }
        case 18:
            switch (variant) {
                case 1:
                    return 10;
                case 0:
                case 2:
                    return 9;
                default:
                    return -1;
            }
        case 19:
            switch (variant) {
                case 1:
                    return 12;
                case 0:
                case 2:
                    return 11;
                default:
                    return -1;
            }
        case 20:
            switch (variant) {
                case 0:
                    return 6;
                case 1:
                    return 8;
                case 2:
                    return 7;
                default:
                    return -1;
            }
        case 21:
            switch (variant) {
                case 0:
                    return 14;
                case 1:
                    return 16;
                case 3:
                    return 15;
                case 2:
                    return 17;
                default:
                    return -1;
            }
        case 22:
            switch (variant) {
                case 1:
                    return 74;
                case 2:
                    return 73;
                case 4:
                    return 71;
                case 5:
                    return 72;
                default:
                    return -1;
            }
        case 23:
            switch (variant) {
                case 0:
                    return 75;
                case 1:
                    return 76;
                default:
                    return -1;
            }
        case 24:
            return 18;
        case 25:
            switch (variant) {
                case 0:
                    return 26;
                case 2:
                    return 27;
                default:
                    return -1;
            }
        case 0:
        case 8:
        case 15:
        case 16:
        default:
            return -1;
    }
}

void Bestiary_UnlockReward(s32 entry, s32 reward)
{
    u32 bit;
    u32 *word = gGameState->bestiaryPairFlags;
    bit = entry * 2 + reward;
    word += bit >> 5;
    *word |= 1 << (bit & 31);
}

void Bestiary_UnlockRewardFromScript(void)
{
    s32 kind, variant, reward;
    if (Script_SeekToKeyword('k'))
        kind = Script_GetValue();
    else
        kind = 0;
    if (Script_SeekToKeyword('c'))
        variant = Script_GetValue();
    else
        variant = 0;
    if (Script_SeekToKeyword('i'))
        reward = Script_GetValue();
    else
        reward = 0;
    Bestiary_UnlockReward(Bestiary_GetEntryForActor(kind, variant), reward);
}

void Bestiary_UnlockEntry(u32 flag)
{
    u8 *word;
    u32 mask;
    if (flag > 0x52)
        return;
    if ((s32)flag <= 31) {
        word = (u8 *)gGameState;
        word += (u32) & ((struct GameState *)0)->bestiaryUnlocked[0];
        mask = 1 << flag;
    } else if ((s32)flag <= 63) {
        word = (u8 *)gGameState;
        word += (u32) & ((struct GameState *)0)->bestiaryUnlocked[1];
        mask = 1 << (flag - 32);
    } else {
        word = (u8 *)gGameState;
        word += (u32) & ((struct GameState *)0)->bestiaryUnlocked[2];
        mask = 1 << (flag - 64);
    }
    *(u32 *)word |= mask;
}

void Bestiary_UnlockEntryFromScript(void)
{
    Bestiary_UnlockEntry(Script_GetKeywordValue(0x6b, -1));
}

u32 Bestiary_IsEntryUnlocked(s32 entry)
{
    u32 *word = gGameState->bestiaryUnlocked;
    s32 wordIndex;
    u32 bit;
    if (entry >= 0) {
        wordIndex = entry >> 5;
    } else {
        wordIndex = -((-entry) >> 5);
    }
    word += wordIndex;
    bit = 1 << (entry & 0x1F);
    if (*word & bit) {
        return 1;
    }
    return 0;
}

u32 Bestiary_IsRewardUnlocked(u32 entry, u32 reward)
{
    u32 *word = gGameState->bestiaryPairFlags;
    u32 flag = (entry << 1) + reward;
    u32 wordIndex = flag >> 5;
    u32 bit;
    if (wordIndex > 7) {
        return 0;
    }
    word += wordIndex;
    bit = 1 << (flag & 0x1F);
    if (*word & bit) {
        return 1;
    }
    return 0;
}

u32 Bestiary_IsGroupComplete(s32 group)
{
    s32 count = Bestiary_GetGroupEntryCount(group);
    s32 base = Bestiary_GetGroupFirstEntry(group);
    s32 i;
    for (i = 0; i < count; i++) {
        if (Bestiary_IsEntryUnlocked(base + i) == 0) {
            return 0;
        }
    }
    return 1;
}

u32 Bestiary_CountUnlockedGroupEntries(s32 group)
{
    s32 count = Bestiary_GetGroupEntryCount(group);
    s32 base = Bestiary_GetGroupFirstEntry(group);
    s32 unlocked = 0;
    s32 i;
    for (i = 0; i < count; i++) {
        if (Bestiary_IsEntryUnlocked(base + i) == 1) {
            unlocked++;
        }
    }
    return unlocked;
}

u32 Bestiary_AreAllEntriesUnlocked(void)
{
    struct GameState *state = gGameState;
    if ((s32)state->bestiaryUnlocked[0] == -1) {
        if ((s32)state->bestiaryUnlocked[1] == -1) {
            if ((state->bestiaryUnlocked[2] & 0x7FFFF) == 0x7FFFF) {
                return 1;
            }
        }
    }
    return 0;
}

void Script_ConsumeOptionalKeywordValue(void)
{
    if (Script_SeekToKeyword(0x6b))
        Script_GetValue();
}

void Bestiary_UpdateCompletionBadges(struct MonsterBook *book)
{
    if (Bestiary_AreAllEntriesUnlocked()) {
        struct AnimSprite *badge = &book->completionBadge;
        badge->flags &= ~1;
    } else {
        struct AnimSprite *badge = &book->completionBadge;
        badge->flags |= 1;
    }
    if (book->mode != 2) {
        if (book->mode == 0) {
            s32 group = Bestiary_GetEntryGroup(book->entry);
            if (Bestiary_IsGroupComplete(group)) {
                struct AnimSprite *badge = &book->groupBadge;
                badge->flags &= ~1;
            } else {
                struct AnimSprite *badge = &book->groupBadge;
                badge->flags |= 1;
            }
        } else {
            book->groupBadge.flags |= 1;
        }
    }
}
