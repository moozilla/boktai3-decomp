#ifdef NONMATCHING
// fuzzy: 88.1%
// Selected-source lookup still has register allocation differences (reviewed 256-byte extent).
#include "modifier_list.h"
s32 Modifier_CollectSources(struct ModifierSelection *actorArg,
                            struct ModifierSelection *selectionArg,
                            struct ModifierRecord *outputArg, u32 includeSelected)
{
    struct ModifierSelection *actor = actorArg;
    struct ModifierSelection *selection = selectionArg;
    struct ModifierRecord *output = outputArg;
    struct ModifierRecord swap, scratch;
    s32 count = 0;
    s32 i;
    struct ModifierRecord *out;
    i = 0;
    if (count < selection->count) {
        out = output;
        do {
            sub_0811E37C(&scratch, ((s16 *)gUnk_02000710->modifierIdsBytes)[i]);
            *out = scratch;
            out++;
            count++;
            i++;
        } while (i < selection->count);
    }
    i = 0;
    {
        s32 selectedCount = selection->count;
        if (i < selectedCount) {
            s16 *ids = (s16 *)gUnk_02000710->modifierIdsBytes;
            s32 selectedId = actor->selectedId;
            do {
                if (*ids == selectedId)
                    goto done;
                ids++;
                i++;
            } while (i < selectedCount);
        }
    }
    if (includeSelected == 1) {
        sub_0811E37C(&scratch, actor->selectedId);
        output[count] = scratch;
        count++;
        {
            s32 pass = 0;
            do {
                s32 nextPass;
                struct ModifierRecord *right;
                struct ModifierRecord *left;
                s32 remaining;
                nextPass = pass + 1;
                right = output + 1;
                left = output;
                remaining = 2;
                do {
                    s32 leftOrder = left->order;
                    if (leftOrder >= 0) {
                        s32 rightOrder = right->order;
                        if (rightOrder >= 0 && leftOrder > rightOrder) {
                            swap = *left;
                            *left = *right;
                            *right = swap;
                        }
                    }
                    right++;
                    left++;
                } while (--remaining >= 0);
                pass = nextPass;
            } while (pass <= 3);
        }
    }
done:
    return count;
}

#else
#include "global.h"
INCLUDE_ASM("asm/nonmatching", Modifier_CollectSources);
#endif
