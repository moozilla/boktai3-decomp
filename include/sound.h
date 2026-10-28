#ifndef GUARD_SOUND_H
#define GUARD_SOUND_H

#include "global.h"

/* RAM objects; each row of symbols/ram.csv with header include/sound.h. */
extern u8 SoundMainRAM_Buffer
    [0x400]; /* MP2K RAM instruction buffer copied with CpuCopy32 size 0x400; next script procedure word is a separate object */

#endif
