#include "global.h"
#include "bosses/vanargand/vanargand_pair_attack.h"
#include "bosses/vanargand/vanargand_stage_colors.h"
void Sound_PlaySong(u32);
void CpuSet(const void *, void *, u32);




// confidence=low: consumes byte AA and installs its callback from the mode table.
extern void *gUnk_08612020[];
void Vanargand_SetPairedMotionParameter(void *, u16);
s32 Vanargand_ApplyRequestedMode(struct VanargandPairAttack *p)
{
    u8 *request = &p->requestedAnimation;
    if ((u8)(*request - 1) > 31)
        return 0;
    {
        u32 a = 2, b = 4;
        p->attackParameter5B8 = a;
        p->attackParameter5BA = b;
    }
    Vanargand_SetPairedMotionParameter(p->pair, p->parameter584);
    {
        void *callback = gUnk_08612020[*request];
        s32 mode = *request;
        Vanargand_StartReaction(p, mode, callback);
    }
    return 1;
}
