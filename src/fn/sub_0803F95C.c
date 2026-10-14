#ifdef NONMATCHING
// fuzzy: 88.1%
// Selected-source lookup still has register allocation differences (reviewed 256-byte extent).
// MATCHING_CONCESSION: register-pinning
#include "modifier_list.h"
s32 Modifier_CollectSources(struct ModifierSelection *actorArg,
                            struct ModifierSelection *selectionArg,
                            struct ModifierRecord *outputArg, u32 includeSelected)
{
    register struct ModifierSelection *actor asm("r10") = actorArg;
    struct ModifierSelection *selection = selectionArg;
    register struct ModifierRecord *output asm("r9") = outputArg;
    struct ModifierRecord swap, scratch;
    register s32 count asm("r8") = 0;
    register s32 i asm("r4");
    struct ModifierRecord *out;
    asm("" : "+m"(includeSelected));
    asm("" : "+r"(actor), "+r"(output));
    asm("" : "+r"(count));
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
            register s32 pass asm("r0") = 0;
            do {
                register s32 nextPass asm("r5");
                register struct ModifierRecord *right asm("r3");
                register struct ModifierRecord *left asm("r2");
                register s32 remaining asm("r4");
                nextPass = pass + 1;
                asm("" : "+r"(output));
                right = output + 1;
                left = output;
                remaining = 2;
                asm("" : : "r"(left), "r"(right), "r"(remaining), "r"(nextPass));
                do {
                    register s32 leftOrder asm("r1") = left->order;
                    if (leftOrder >= 0) {
                        register s32 rightOrder asm("r0") = right->order;
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
                asm("" : "+r"(pass));
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
