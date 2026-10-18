#define SoundDelay_Update sub_08138FE0
#include "global.h"
struct SoundDelay {
    void (*update)(struct SoundDelay *);
    u8 unknown04[0xd0];
    u8 entered;
    u8 unknownD5[3];
    u16 timer;
};
void Sound_PlaySong(u32);
void sub_08138CDC(struct SoundDelay *);
void sub_08138FE0(struct SoundDelay *p)
{
    if (p->entered) {
        p->timer = 0;
        Sound_PlaySong(0x1b5);
        p->entered = 0;
    }
    if (p->timer > 30) {
        p->update = sub_08138CDC;
        p->entered = 1;
    } else {
        p->timer++;
    }
}
