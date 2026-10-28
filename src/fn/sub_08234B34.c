#include "global.h"
#include "render.h"
#include "engine_globals.h"
void HealthRestoreEffect_StartParticleBurst(u8 *);
void Sound_PlaySong(u32);
void Sound_ResumePersistentSongs(void);
void Task_Kill(u8 *);
static inline void clear(u32 *dst, u32 mask)
{
    *dst &= mask;
}
#define H(o) (*(u16 *)(p + (o)))
void HealthRestoreEffect_UpdateCompletion(u8 *p)
{
    if (H(0x270) == 30) {
        HealthRestoreEffect_StartParticleBurst(p);
        Sound_PlaySong(0x285);
        *(u32 *)(p + 0x24) |= 1;
    }
    H(0x270)++;
    if (H(0x270) > 59) {
        u32 mask, value;
        u32 *flags;
        Sound_ResumePersistentSongs();
        flags = &gEnginePauseFlags;
        mask = ~4;
        clear(flags, mask);
        value = gEngineFlags;
        mask += 3;
        gEngineFlags = value & mask;
        Task_Kill(p);
    }
}
