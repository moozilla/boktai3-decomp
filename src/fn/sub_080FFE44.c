#include "global.h"
#include "display_animation.h"
#include "player_cue_sequence.h"

struct WoodenCrateStep {
    u32 flags;
    u8 unk4[3];
    u8 mode;
    u8 unk8[8];
    u16 frame;
    u8 unk12[0x1A];
    struct DisplaySequenceState sequence;
    u8 unk3C[0x96];
    u16 counterD2;
    u8 resetPending;
};
void WoodenCrate_UpdatePaletteTimers(void *);
void WoodenCrate_CheckTrigger(void *);
void WoodenCrate_UpdateShake(void *);

void WoodenCrate_Update(void *unused, struct WoodenCrateStep *crate)
{
    if (crate->resetPending) {
        crate->counterD2 = 0;
        crate->flags &= ~1;
        crate->resetPending = 0;
    }
    PlayerCue_StepSequence((struct ContactSwarmSprite *)crate, &crate->sequence);
    WoodenCrate_UpdatePaletteTimers(crate);
    WoodenCrate_CheckTrigger(crate);
    WoodenCrate_UpdateShake(crate);
    crate->counterD2++;
}
