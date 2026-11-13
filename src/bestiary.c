#include "bestiary.h"

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
    s32 result;
    switch (kind) {
        case 1:
            switch (variant) {
                case 0:
                    result = 23;
                    break;
                case 1:
                    result = 24;
                    break;
                case 2:
                    result = 25;
                    break;
                default:
                    goto missing;
            }
            break;
        case 2:
            switch (variant) {
                case 1:
                    result = 5;
                    break;
                case 0:
                case 2:
                    result = 4;
                    break;
                default:
                    goto missing;
            }
            break;
        case 3:
            switch (variant) {
                case 0:
                    result = 44;
                    break;
                case 1:
                    result = 45;
                    break;
                case 3:
                    result = 46;
                    break;
                default:
                    goto missing;
            }
            break;
        case 4:
            switch (variant) {
                case 1:
                    result = 20;
                    break;
                case 2:
                    result = 21;
                    break;
                case 3:
                    result = 22;
                    break;
                default:
                    goto missing;
            }
            break;
        case 5:
            switch (variant) {
                case 0:
                    result = 37;
                    break;
                case 5:
                    result = 43;
                    break;
                case 12:
                    result = 40;
                    break;
                default:
                    goto missing;
            }
            break;
        case 6:
            switch (variant) {
                case 0:
                    result = 38;
                    break;
                case 12:
                    result = 41;
                    break;
                default:
                    goto missing;
            }
            break;
        case 7:
            switch (variant) {
                case 0:
                    result = 39;
                    break;
                case 12:
                    result = 42;
                    break;
                default:
                    goto missing;
            }
            break;
        case 9:
            result = 13;
            break;
        case 10:
            result = 47;
            break;
        case 11:
            switch (variant) {
                case 0:
                    result = 48;
                    break;
                case 1:
                    result = 49;
                    break;
                default:
                    goto missing;
            }
            break;
        case 12:
        case 13:
            switch (variant) {
                case 0:
                    result = 50;
                    break;
                case 1:
                    result = 51;
                    break;
                default:
                    goto missing;
            }
            break;
        case 14:
            result = 19;
            break;
        case 17:
            switch (variant) {
                case 0:
                    result = 68;
                    break;
                case 1:
                    result = 69;
                    break;
                case 2:
                    result = 70;
                    break;
                default:
                    goto missing;
            }
            break;
        case 18:
            switch (variant) {
                case 1:
                    result = 10;
                    break;
                case 0:
                case 2:
                    result = 9;
                    break;
                default:
                    goto missing;
            }
            break;
        case 19:
            switch (variant) {
                case 1:
                    result = 12;
                    break;
                case 0:
                case 2:
                    result = 11;
                    break;
                default:
                    goto missing;
            }
            break;
        case 20:
            switch (variant) {
                case 0:
                    result = 6;
                    break;
                case 1:
                    result = 8;
                    break;
                case 2:
                    result = 7;
                    break;
                default:
                    goto missing;
            }
            break;
        case 21:
            switch (variant) {
                case 0:
                    result = 14;
                    break;
                case 1:
                    result = 16;
                    break;
                case 3:
                    result = 15;
                    break;
                case 2:
                    result = 17;
                    break;
                default:
                    goto missing;
            }
            break;
        case 22:
            switch (variant) {
                case 1:
                    result = 74;
                    break;
                case 2:
                    result = 73;
                    break;
                case 4:
                    result = 71;
                    break;
                case 5:
                    result = 72;
                    break;
                default:
                    goto missing;
            }
            break;
        case 23:
            switch (variant) {
                case 0:
                    result = 75;
                    break;
                case 1:
                    result = 76;
                    break;
                default:
                    goto missing;
            }
            break;
        case 24:
            result = 18;
            break;
        case 25:
            switch (variant) {
                case 0:
                    result = 26;
                    break;
                case 2:
                    result = 27;
                    break;
                default:
                    goto missing;
            }
            break;
        case 0:
        case 8:
        case 15:
        case 16:
        default:
        missing:
            result = -1;
            break;
    }
    return result;
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
        struct SceneInputWidget *badge = &book->completionBadge;
        badge->flags &= ~1;
    } else {
        struct SceneInputWidget *badge = &book->completionBadge;
        badge->flags |= 1;
    }
    if (book->mode != 2) {
        if (book->mode == 0) {
            s32 group = Bestiary_GetEntryGroup(book->entry);
            if (Bestiary_IsGroupComplete(group)) {
                struct SceneInputWidget *badge = &book->groupBadge;
                badge->flags &= ~1;
            } else {
                struct SceneInputWidget *badge = &book->groupBadge;
                badge->flags |= 1;
            }
        } else {
            book->groupBadge.flags |= 1;
        }
    }
}
