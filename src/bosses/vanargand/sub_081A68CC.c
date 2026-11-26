#include "bosses/vanargand/vanargand_pair_attack.h"

void Sound_StopSong(s32);

void Vanargand_StopActionSequence(struct VanargandActionSequence *sequence)
{
    sequence->timer = 0;
    sequence->step = 0;
    Sound_StopSong(0x583);
}
