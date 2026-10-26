#include "modifier_list.h"

s32 Modifier_CollectSources(struct ModifierSelection *, struct ModifierSelection *,
                            struct ModifierRecord *, u32);

void Modifier_AddSelection(struct ModifierSelection *p, u32 n)
{
    p->mask |= 1 << n;
    p->count += 1;
}

void Modifier_RemoveSelection(struct ModifierSelection *p, u32 n)
{
    p->mask &= ~(1 << n);
    p->count -= 1;
}

u32 Modifier_TestSelection(struct ModifierSelection *p, u32 n)
{
    return p->mask & (1 << n);
}

u32 Modifier_ReadPackedValue(u32 k, s32 n)
{
    u32 sh = n * 3 * 2;
    u32 m = 0x3f;
    u32 r;
    s32 v;
    switch (k) {
        case 0:
            v = gGameState->unk7B8[0];
            r = (v >> sh) & m;
            break;
        case 1:
            v = gGameState->unk7B8[2];
            r = (v >> sh) & m;
            break;
        case 2:
            v = gGameState->unk7B8[1];
            r = (v >> sh) & m;
            break;
        case 3:
            v = gGameState->unk7B8[3];
            r = (v >> sh) & m;
            break;
        case 4:
            v = gGameState->unk81C;
            r = (v >> sh) & m;
            break;
        case 5:
            v = gGameState->unk7F8;
            r = (v >> sh) & m;
            break;
        default:
            r = 0;
            break;
    }
    return r;
}

/* Modifier_EnumerateSelection: Fills an output array with selected bit indices in ascending order and returns the count */

#define Modifier_EnumerateSelection sub_0803F8F4

static inline void Fill(s32 *p, s32 v, s32 n)
{
    s32 *q = p + n - 1;
    do {
        *q = v;
        q--;
    } while ((s32)q >= (s32)p);
}

s32 Modifier_EnumerateSelection(struct ModifierSelection *p, s32 *out)
{
    s32 *w;
    s32 n, i;
    Fill(out, -1, 16);
    n = 0;
    for (i = 0, w = out; i <= 23; i++) {
        if (Modifier_TestSelection(p, i) && n <= p->count) {
            *w++ = i;
            n++;
        }
    }
    return n;
}

s32 Div(s32, s32);

s32 Modifier_ApplyAmount(s32 a, s32 b, s32 c)
{
    switch (b) {
        case 0:
        case 7:
        case 8:
        case 9:
        case 10:
        case 12:
            if (c != 0)
                a = Div(a * c, 10);
            break;
        default:
            a = a + c;
            break;
    }
    return a;
}

s32 Modifier_CollectSources(struct ModifierSelection *actor, struct ModifierSelection *selection,
                            struct ModifierRecord *output, u32 includeSelected)
{
    struct ModifierRecord swap, scratch;
    s32 count = 0;
    s32 i;
    i = 0;
    if (count < selection->count) {
        do {
            ScriptRecord_FindModifierRecord(&scratch, ((s16 *)gGameState->modifierIdsBytes)[i]);
            output[count] = scratch;
            count++;
            i++;
        } while (i < selection->count);
    }
    i = 0;
    {
        s32 selectedCount = selection->count;
        if (i < selectedCount) {
            struct GameState *state = gGameState;
            s32 selectedId = actor->selectedId;
            s16 *ids = (s16 *)state->modifierIdsBytes;
            do {
                if (*ids == selectedId)
                    goto done;
                ids++;
                i++;
            } while (i < selectedCount);
        }
    }
    if (includeSelected == 1) {
        ScriptRecord_FindModifierRecord(&scratch, actor->selectedId);
        output[count] = scratch;
        count++;
        {
            s32 pass;
            for (pass = 0; pass < 4; pass++) {
                s32 j;
                for (j = 0; j < 3; j++) {
                    s32 leftOrder = output[j].order;
                    if (leftOrder >= 0) {
                        s32 rightOrder = output[j + 1].order;
                        if (rightOrder >= 0 && leftOrder > rightOrder) {
                            swap = output[j];
                            output[j] = output[j + 1];
                            output[j + 1] = swap;
                        }
                    }
                }
            }
        }
    }
done:
    return count;
}

s32 Modifier_ApplyToStat(struct ModifierSelection *actor, struct ModifierSelection *selection,
                         s32 value, u32 includeSelected, u32 kindArg)
{
    u32 kind = kindArg;
    s32 acc = value;
    /* Original frame reserves an unused eight-byte prefix before the records. */
    struct {
        u8 unused[8];
        struct ModifierRecord records[8];
    } workspace;
    struct ModifierRecord *record = workspace.records;
    s32 i;
    s32 result;
    for (i = 0; i < 8; i++)
        record[i].order = -1;
    result = Modifier_CollectSources(actor, selection, record, includeSelected);
    if (result > 0) {
        s32 count = result;
        do {
            if (record->kind == kind)
                acc = Modifier_ApplyAmount(acc, kind, record->amount);
            record++;
        } while (--count != 0);
    }
    if (acc > 999)
        acc = 999;
    return acc;
}

s32 Modifier_ApplyKindSix(struct ModifierSelection *a, struct ModifierSelection *b, s32 c, u32 d)
{
    c = Modifier_ApplyToStat(a, b, c, d, 6);
    if (c > 999)
        c = 999;
    return c;
}

s32 Modifier_ApplyKindZero(struct ModifierSelection *a, struct ModifierSelection *b, s32 c, u32 d)
{
    c = Modifier_ApplyToStat(a, b, c, d, 0);
    if (c > 999)
        c = 999;
    return c;
}

s32 Modifier_ApplyKindSeven(struct ModifierSelection *a, struct ModifierSelection *b, s32 c, u32 d)
{
    c = Modifier_ApplyToStat(a, b, c, d, 7);
    if (c > 999)
        c = 999;
    return c;
}

s32 Modifier_ApplyPositiveKindThree(struct ModifierSelection *a, struct ModifierSelection *b, s32 c,
                                    u32 d)
{
    s32 r;
    if (c <= 0) {
        r = 0;
    } else {
        c = Modifier_ApplyToStat(a, b, c, d, 3);
        if (c > 999)
            c = 999;
        r = c;
    }
    return r;
}

s32 Modifier_ApplyKindEight(struct ModifierSelection *a, struct ModifierSelection *b, s32 c, u32 d)
{
    c = Modifier_ApplyToStat(a, b, c, d, 8);
    if (c > 999)
        c = 999;
    return c;
}

s32 Modifier_ApplyKindNine(struct ModifierSelection *a, struct ModifierSelection *b, s32 c, u32 d)
{
    c = Modifier_ApplyToStat(a, b, c, d, 9);
    if (c > 999)
        c = 999;
    return c;
}

s32 Modifier_ApplyKindTen(struct ModifierSelection *a, struct ModifierSelection *b, s32 c, u32 d)
{
    c = Modifier_ApplyToStat(a, b, c, d, 10);
    if (c > 999)
        c = 999;
    return c;
}

s32 Modifier_ApplyKindTwelve(struct ModifierSelection *a, struct ModifierSelection *b, s32 c, u32 d)
{
    c = Modifier_ApplyToStat(a, b, c, d, 12);
    if (c > 999)
        c = 999;
    return c;
}

void ScriptRecord_FindExtendedModifier(u16 *, s32);

void Modifier_LoadFirstSource(struct ModifierSelection *p)
{
    u16 buf[8];
    ScriptRecord_FindExtendedModifier(buf, p->sourceIds[0]);
    p->values0[0] = buf[3];
    p->values0[1] = buf[2];
}

void ScriptRecord_FindByteModifier(struct ModifierByteSourceInfo *, s32);

void Modifier_LoadSecondSource(struct ModifierSelection *p, const struct ModifierSelection *current)
{
    struct ModifierByteSourceInfo info;
    ScriptRecord_FindByteModifier(&info, p->sourceIds[1]);
    p->values0[2] = info.kindZero;
    p->values0[3] = info.kindSeven;
    p->values0[4] = info.kindSix;
    p->capacity = info.capacity;
}

void ScriptRecord_FindModifier(u16 *, s32);

void Modifier_LoadThirdSource(struct ModifierSelection *p)
{
    u16 buf[6];
    ScriptRecord_FindModifier(buf, p->sourceIds[2]);
    p->valuesE[0] = buf[1];
    p->valuesE[1] = buf[2];
    p->valuesE[2] = buf[3];
}

s32 ScriptRecord_FindAttributeModifier(struct ModifierItemInfo *, s32);

void Modifier_LoadFourthSource(struct ModifierSelection *selection)
{
    struct ModifierItemInfo info;
    if (ScriptRecord_FindAttributeModifier(&info, selection->sourceIds[3]) < 0) {
        selection->valuesE[3] = 0xffff;
        selection->valuesE[4] = -1;
    } else {
        selection->valuesE[3] = info.value6;
        selection->valuesE[4] = info.value4;
    }
}

// MATCHING_CONCESSION: register-pinning
void ModifierMenu_SelectPendingSource(struct ModifierMenu *menu)
{
    struct ModifierSelection *pending = &menu->pending;
    switch (menu->mode) {
        case 0: {
            s32 index = menu->choice.cursor.index;
            s32 category = menu->choice.cursor.category;
            if (category == 0) {
                pending->sourceIds[0] = index;
                Modifier_LoadFirstSource(pending);
            } else if (category == 1) {
                pending->sourceIds[1] = index;
                goto secondSource;
            } else if (category == 2) {
                pending->sourceIds[2] = index;
                Modifier_LoadThirdSource(pending);
            } else {
                pending->sourceIds[3] = index;
                Modifier_LoadFourthSource(pending);
            }
            break;
        }
        case 1: {
            register s32 category asm("r1") = menu->choice.cursor.category;
            s32 index = menu->choice.cursor.index;
            category = category * 3 * 2 + index;
            pending->selectedId = category;
        secondSource:
            Modifier_LoadSecondSource(pending, &menu->current);
            break;
        }
        case 2: {
            register s32 category asm("r1") = menu->choice.cursor.category;
            s32 index = menu->choice.cursor.index;
            category = category * 3 * 2 + index;
            pending->secondaryId = category;
            break;
        }
    }
}

s32 ModifierMenu_CommitFirstSource(struct ModifierMenu *p)
{
    struct ModifierSelection *s = &p->pending;
    struct ModifierSelection *d = &p->current;
    u16 v;
    d->values0[0] = s->values0[0];
    d->values0[1] = s->values0[1];
    v = s->sourceIds[0];
    d->sourceIds[0] = v;
    gGameState->unk7A0[0] = v;
    return 0;
}

s32 ModifierMenu_CommitSecondSource(struct ModifierMenu *p)
{
    struct ModifierSelection *s = &p->pending;
    struct ModifierSelection *d = &p->current;
    u16 v;
    d->values0[2] = s->values0[2];
    d->values0[3] = s->values0[3];
    d->values0[4] = s->values0[4];
    d->capacity = s->capacity;
    v = s->sourceIds[1];
    d->sourceIds[1] = v;
    gGameState->unk7A0[2] = v;
    return 0;
}

void ModifierMenu_ReconcileSelection(struct ModifierMenu *menu)
{
    struct ModifierSelection *current = &menu->current;
    s32 count, index;
    u32 mask;
    if (current->count > current->capacity) {
        count = 0;
        mask = 0;
        index = 0;
        for (;;) {
            if (index > 31)
                break;
            if (Modifier_TestSelection(current, index)) {
                mask |= 1 << index;
                count++;
                if (count >= current->capacity)
                    break;
            }
            index++;
        }
        current->count = count;
        current->mask = mask;
        if (menu->mode == 0)
            menu->changed = 1;
    }
    for (index = 0; index <= 3; index++)
        ((s16 *)gGameState->modifierIdsBytes)[index] = -1;
    for (count = 0, index = 0; index <= 31; index++) {
        if (Modifier_TestSelection(current, index)) {
            ((s16 *)gGameState->modifierIdsBytes)[count] = index;
            count++;
        }
    }
}

s32 ModifierMenu_CommitThirdSource(struct ModifierMenu *p)
{
    struct ModifierSelection *s = &p->pending;
    struct ModifierSelection *d = &p->current;
    u16 v;
    d->valuesE[0] = s->valuesE[0];
    d->valuesE[1] = s->valuesE[1];
    d->valuesE[2] = s->valuesE[2];
    v = s->sourceIds[2];
    d->sourceIds[2] = v;
    gGameState->unk7A0[1] = v;
    return 0;
}

s32 ModifierMenu_CommitFourthSource(struct ModifierMenu *p)
{
    struct ModifierSelection *s = &p->pending;
    struct ModifierSelection *d = &p->current;
    u16 v;
    d->valuesE[3] = s->valuesE[3];
    d->valuesE[4] = s->valuesE[4];
    v = s->sourceIds[3];
    d->sourceIds[3] = v;
    gGameState->unk7A0[3] = v;
    return 0;
}

// MATCHING_CONCESSION: register-pinning
struct ModifierMenu *ModifierMenu_CommitSelection(struct ModifierMenu *menu)
{
    register struct ModifierMenu *base asm("r2") = menu;
    struct ModifierSelection *pending;
    struct ModifierSelection *current;
    pending = &base->pending;
    current = &base->current;
    current->selectedId = pending->selectedId;
    current->count = pending->count;
    current->mask = pending->mask;
    return menu;
}

s32 ModifierMenu_ToggleSelection(struct ModifierMenu *menu)
{
    struct ModifierSelection *pending = &menu->pending;
    struct ModifierSelection *current = &menu->current;
    s32 id;
    current->selectedId = pending->selectedId;
    id = pending->selectedId;
    if (Modifier_TestSelection(current, id))
        Modifier_RemoveSelection(current, id);
    else {
        if (current->count >= current->capacity)
            return -1;
        Modifier_AddSelection(current, id);
    }
    return 0;
}

s32 ModifierMenu_CommitSecondarySource(struct ModifierMenu *p)
{
    struct ModifierSelection *s = &p->pending;
    struct ModifierSelection *d = &p->current;
    u16 v = s->secondaryId;
    d->secondaryId = v;
    gGameState->unk81A = v;
    return 0;
}

extern u32 gUnk_030053F0;

void Registry1745_SetMenuRequest(u32);

static inline u32 TestFlags(u32 mask)
{
    return (gEngineFlags | gUnk_030053F0) & mask;
}

void ModifierMenu_CloseScript(struct ModifierMenu *menu)
{
    void **script = &menu->script;
    if (*script) {
        Script_Call(*script, 0);
        *script = 0;
    }
    if (TestFlags(0x800))
        Registry1745_SetMenuRequest(1);
}

extern const u8 gUnk_086058E8[];

void ScreenTransition_SendCommand(u32, u32, u32, u32, u32, u32, u32);

void ModifierMenu_BeginClose(struct ModifierMenu *p)
{
    p->state = 3;
    p->stateFrame = 0;
    if (p->choice.cursor.category >= gUnk_086058E8[p->mode])
        p->choice.cursor.category = 0;
    ScreenTransition_SendCommand(1, 5, 4, 4, 4, 0xFFFF, 0);
}

u32 StateFlags_GetCategoryBit(s32, s32);

s32 ModifierMenu_ApplyPendingSource(struct ModifierMenu *menu)
{
    s32 result = -1;
    switch (menu->mode) {
        case 0:
            if (!StateFlags_GetCategoryBit(menu->choice.cursor.category, menu->choice.cursor.index))
                break;
            if (menu->choice.cursor.category == 0)
                result = ModifierMenu_CommitFirstSource(menu);
            else if (menu->choice.cursor.category == 1) {
                result = ModifierMenu_CommitSecondSource(menu);
                ModifierMenu_ReconcileSelection(menu);
            } else if (menu->choice.cursor.category == 2)
                result = ModifierMenu_CommitThirdSource(menu);
            else
                result = ModifierMenu_CommitFourthSource(menu);
            break;
        case 1: {
            s32 category = menu->choice.cursor.category;
            s32 index = menu->choice.cursor.index;
            if (!StateFlags_GetCategoryBit(4, category * 3 * 2 + index))
                break;
            result = ModifierMenu_ToggleSelection(menu);
            ModifierMenu_ReconcileSelection(menu);
            break;
        }
        case 2: {
            s32 category = menu->choice.cursor.category;
            s32 index = menu->choice.cursor.index;
            if (StateFlags_GetCategoryBit(5, category * 3 * 2 + index))
                result = ModifierMenu_CommitSecondarySource(menu);
            break;
        }
    }
    return result;
}

void sub_08215EB4(s32, s32, s32, s32, s32, s32, s32, s32);

void Text_DrawNumber(s32, s32, s32, s32, s32, s32, s32);

void ModifierMenu_DrawStatComparison(s32 a, s32 b, s32 c, s32 d, u32 k)
{
    s32 df;
    s32 f;
    switch (k) {
        case 1:
        case 8:
        case 10:
            df = a - b;
            break;
        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 9:
        default:
            df = b - a;
            break;
    }
    f = 0x31;
    if (df <= 0) {
        f = 0x11;
        if (df < 0)
            f = 0x21;
    }
    if (k == 9)
        goto chk;
    if (k != 10)
        goto B;
chk:
    if (a < 0) {
        if (b < 0) {
            sub_08215EB4(2, c, d, 3, 1, 0x10, 0, 0);
            return;
        }
    }
B:
    Text_DrawNumber(2, c, d, b, f, 3, 0);
}

void sub_08215EB4(s32, s32, s32, s32, s32, s32, s32, s32);

void ModifierMenu_DrawUnavailableStat(u32 a, u32 b, u32 c)
{
    sub_08215EB4(2, a, b, c, 1, 0x10, 0, 0);
}

void Bg_SetTilemapEntry(s32, s32, s32, s32, s32, s32);

void ModifierMenu_DrawCapacity(s32 a, s32 b, s32 count, s32 y, s32 width)
{
    s32 diff;
    s32 color;
    s32 i;
    diff = count - b;
    sub_08215EB4(2, y, width, 4, 1, 1, 0, 0);
    color = 0x3b;
    if (diff <= 0) {
        color = 0x1b;
        if (diff < 0)
            color = 0x2b;
    }
    i = 0;
    if (i < count) {
        do {
            s32 c;
            c = color;
            if (i >= a)
                goto no_increment;
            c++;
        no_increment:
            Bg_SetTilemapEntry(2, y + i, width, c, 0, 0);
            i++;
        } while (i < count);
    }
}

void ModifierMenu_DrawFirstSourceStats(struct ModifierMenu *menu)
{
    struct ModifierSelection *pending = &menu->pending;
    struct ModifierSelection *current = &menu->current;
    s32 first = Modifier_ApplyPositiveKindThree(pending, current, current->values0[0], 0);
    s32 second = Modifier_ApplyKindNine(pending, current, current->values0[1], 0);
    s32 nextFirst =
        Modifier_ApplyPositiveKindThree(pending, current, pending->values0[0], menu->mode);
    s32 nextSecond = Modifier_ApplyKindNine(pending, current, pending->values0[1], menu->mode);
    switch (menu->state) {
        case 0:
        case 2:
            ModifierMenu_DrawStatComparison(first, first, 21, 4, 0);
            ModifierMenu_DrawStatComparison(second, second, 21, 5, 1);
            break;
        case 1:
            if (StateFlags_GetCategoryBit(menu->choice.cursor.category,
                                          menu->choice.cursor.index)) {
                ModifierMenu_DrawStatComparison(first, nextFirst, 8, 13, 0);
                ModifierMenu_DrawStatComparison(second, nextSecond, 8, 14, 1);
            } else {
                ModifierMenu_DrawUnavailableStat(8, 13, 3);
                ModifierMenu_DrawUnavailableStat(8, 14, 3);
            }
            break;
    }
}

// MATCHING_CONCESSION: register-pinning
void ModifierMenu_DrawSecondSourceStats(struct ModifierMenu *menu)
{
    struct ModifierSelection *pending = &menu->pending;
    struct ModifierSelection *current = &menu->current;
    s32 first = Modifier_ApplyKindZero(pending, current, current->values0[2], 0);
    s32 second = Modifier_ApplyKindSeven(pending, current, current->values0[3], 0);
    s32 third = Modifier_ApplyKindSix(pending, current, current->values0[4], 0);
    s32 count = current->count;
    s32 capacity = current->capacity;
    s32 nextFirst;
    s32 nextSecond;
    s32 nextThird;
    s32 nextCapacity;
    if (menu->mode == 1) {
        nextFirst = Modifier_ApplyKindZero(pending, current, current->values0[2], 1);
        nextSecond = Modifier_ApplyKindSeven(pending, current, current->values0[3], menu->mode);
        nextThird = Modifier_ApplyKindSix(pending, current, current->values0[4], menu->mode);
        nextCapacity = current->capacity;
    } else {
        nextFirst = Modifier_ApplyKindZero(pending, current, pending->values0[2], menu->mode);
        nextSecond = Modifier_ApplyKindSeven(pending, current, pending->values0[3], menu->mode);
        nextThird = Modifier_ApplyKindSix(pending, current, pending->values0[4], menu->mode);
        nextCapacity = pending->capacity;
    }
    switch (menu->state) {
        case 0:
        case 2:
            do {
                ModifierMenu_DrawStatComparison(first, first, 21, 6, 2);
            } while (0);
            ModifierMenu_DrawStatComparison(second, second, 21, 7, 3);
            ModifierMenu_DrawStatComparison(third, third, 21, 8, 4);
            ModifierMenu_DrawCapacity(count, capacity, capacity, 21, 9);
            break;
        case 1:
            if (menu->mode == 0) {
                if (StateFlags_GetCategoryBit(menu->choice.cursor.category,
                                              menu->choice.cursor.index)) {
                    goto validPreview;
                }
                goto invalidPreview;
            } else if (menu->mode == 1) {
                if (!StateFlags_GetCategoryBit(4, menu->choice.cursor.index +
                                                      ((menu->choice.cursor.category * 3) * 2))) {
                    goto invalidPreview;
                }
                do {
                validPreview:
                    ModifierMenu_DrawStatComparison(first, nextFirst, 8, 13, 2);
                    ModifierMenu_DrawStatComparison(second, nextSecond, 8, 14, 3);
                    ModifierMenu_DrawStatComparison(third, nextThird, 8, 15, 4);
                } while (0);
                ModifierMenu_DrawCapacity(count, capacity, nextCapacity, 8, 16);
                break;
            invalidPreview:
                ModifierMenu_DrawUnavailableStat(8, 13, 3);
                ModifierMenu_DrawUnavailableStat(8, 14, 3);
                ModifierMenu_DrawUnavailableStat(8, 15, 3);
                ModifierMenu_DrawUnavailableStat(8, 16, 3);
            } else {
                ModifierMenu_DrawStatComparison(first, first, 8, 13, 2);
                ModifierMenu_DrawStatComparison(second, second, 8, 14, 3);
                ModifierMenu_DrawStatComparison(third, third, 8, 15, 4);
                ModifierMenu_DrawCapacity(count, capacity, capacity, 8, 16);
            }
            break;
    }
}

void ModifierMenu_DrawThirdSourceStats(struct ModifierMenu *menu)
{
    struct ModifierSelection *pending = &menu->pending;
    struct ModifierSelection *current = &menu->current;
    s32 first = current->valuesE[0];
    s32 second = Modifier_ApplyKindTwelve(pending, current, current->valuesE[1], 0);
    s32 third = Modifier_ApplyKindEight(pending, current, current->valuesE[2], 0);
    s32 nextFirst = pending->valuesE[0];
    s32 nextSecond = Modifier_ApplyKindTwelve(pending, current, pending->valuesE[1], menu->mode);
    s32 nextThird = Modifier_ApplyKindEight(pending, current, pending->valuesE[2], menu->mode);
    switch (menu->state) {
        case 0:
        case 2:
            ModifierMenu_DrawStatComparison(first, first, 21, 10, 6);
            ModifierMenu_DrawStatComparison(second, second, 21, 11, 7);
            ModifierMenu_DrawStatComparison(third, third, 21, 12, 8);
            break;
        case 1:
            if (StateFlags_GetCategoryBit(menu->choice.cursor.category,
                                          menu->choice.cursor.index)) {
                ModifierMenu_DrawStatComparison(first, nextFirst, 8, 13, 6);
                ModifierMenu_DrawStatComparison(second, nextSecond, 8, 14, 7);
                ModifierMenu_DrawStatComparison(third, nextThird, 8, 15, 8);
            } else {
                ModifierMenu_DrawUnavailableStat(8, 13, 3);
                ModifierMenu_DrawUnavailableStat(8, 14, 3);
                ModifierMenu_DrawUnavailableStat(8, 15, 3);
            }
            break;
    }
}

void Bg_SetTilemapEntry(s32, s32, s32, s32, s32, s32);

void ModifierMenu_DrawFourthSourceStats(struct ModifierMenu *menu)
{
    struct ModifierSelection *pending = &menu->pending;
    struct ModifierSelection *current = &menu->current;
    s32 first = Modifier_ApplyPositiveKindThree(pending, current, current->valuesE[3], menu->mode);
    s32 second = Modifier_ApplyKindTen(pending, current, current->valuesE[4], menu->mode);
    s32 nextFirst =
        Modifier_ApplyPositiveKindThree(pending, current, pending->valuesE[3], menu->mode);
    s32 nextSecond = Modifier_ApplyKindTen(pending, current, pending->valuesE[4], menu->mode);
    switch (menu->state) {
        case 0:
        case 2: {
            u32 zero, one;
            ModifierMenu_DrawStatComparison(first, first, 25, 4, 9);
            ModifierMenu_DrawStatComparison(second, second, 25, 5, 10);
            zero = 0;
            Bg_SetTilemapEntry(2, 24, 4, 0x1d, zero, zero);
            one = 1;
            Bg_SetTilemapEntry(2, 28, 4, 0x1d, one, zero);
            Bg_SetTilemapEntry(2, 24, 5, 0x1d, zero, zero);
            Bg_SetTilemapEntry(2, 28, 5, 0x1d, one, zero);
            break;
        }
        case 1: {
            u32 zero, one;
            if (StateFlags_GetCategoryBit(menu->choice.cursor.category,
                                          menu->choice.cursor.index)) {
                ModifierMenu_DrawStatComparison(first, nextFirst, 8, 13, 9);
                ModifierMenu_DrawStatComparison(second, nextSecond, 8, 14, 10);
            } else {
                ModifierMenu_DrawUnavailableStat(8, 13, 3);
                ModifierMenu_DrawUnavailableStat(8, 14, 3);
            }
            zero = 0;
            Bg_SetTilemapEntry(2, 7, 13, 0x1d, zero, zero);
            one = 1;
            Bg_SetTilemapEntry(2, 11, 13, 0x1d, one, zero);
            Bg_SetTilemapEntry(2, 7, 14, 0x1d, zero, zero);
            Bg_SetTilemapEntry(2, 11, 14, 0x1d, one, zero);
            break;
        }
    }
}

void Bg_SetupLayers(u32, u32, u8 *, u32, u32, u32, u32 *);

void CpuSet(const void *, void *, u32);

u32 ModifierMenu_LoadBackground(struct ModifierMenu *p, u32 x)
{
    u32 v;
    u32 z;
    v = x;
    p->backgroundResource = ScriptProc_Start(0xC091, 0xCE15);
    z = 0;
    Bg_SetupLayers(1, 0, p->backgroundResource, 0, z, 1, &v);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, z);
    p->tilemap = (u8 *)ScriptProc_Start(0x92B3, 0xA2F6) + 0x14;
    CpuSet(p->tilemap, (void *)0x03004FC0, 0x100);
    return 0;
}

u32 ModifierMenu_StartBackgroundScript(struct ModifierMenu *p)
{
    u32 v;
    v = 3;
    p->backgroundScript = ScriptProc_Start(0xC091, 0xA597);
    Bg_SetupLayers(0, 0, p->backgroundScript, 0, 0, 1, &v);
    return 0;
}

void ModifierMenu_ClearBackground(void)
{
    Bg_ClearTilemap(0);
}

void Bg_ResetScale(u32);

void ModifierMenu_ResetBackground(struct ModifierMenu *menu)
{
    Bg_ResetScale(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
}

void Bg_SetLayerMap(u32, void *, u32);

void ModifierMenu_DrawBackground(struct ModifierMenu *menu)
{
    s32 visible;
    /* do-while(0) only keeps agbcc's block layout */
    do {
        switch (menu->state) {
            case 1:
                visible = 1;
                break;
            case 2:
                visible = 0;
                ModifierMenu_ResetBackground(menu);
                break;
            case 0:
            default:
                visible = 0;
                break;
        }
        Bg_SetLayerMap(2, menu->backgroundResource, visible);
        BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    } while (0);
}
