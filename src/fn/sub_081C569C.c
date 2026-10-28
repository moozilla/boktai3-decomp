#include "global.h"
#include "input.h"
struct ChallengeMenu {
    u8 pad000[0x2E2];
    u8 repeatTimer;
    u8 enabledMask;
    u32 prevKeys;
    u8 pad2E8[0x300 - 0x2E8];
    s32 selection;
};
void ChallengeMenu_StartFadeOut(struct ChallengeMenu *);
void Sound_PlaySong(s32);
void SceneTextSelector_UpdatePanel(struct ChallengeMenu *);
void sub_081C569C(struct ChallengeMenu *p)
{
    u16 *keys = gPadState;
    u8 *timer;
    u32 repeatMask;
    u32 pendingKeys;
    if (keys[1] & 1) {
        if (((p->enabledMask >> (p->selection - 1)) & 1) || p->selection == 0) {
            ChallengeMenu_StartFadeOut(p);
            Sound_PlaySong(0xDD);
            return;
        }
        Sound_PlaySong(0x192);
    } else if (keys[1] & 2) {
        p->selection = 5;
        Sound_PlaySong(0xDE);
        ChallengeMenu_StartFadeOut(p);
        return;
    }
    if (keys[0] & 0x40) {
        if (!(p->prevKeys & 0x40))
            p->repeatTimer = 0;
        timer = &p->repeatTimer;
        if (++*timer > 9) {
            pendingKeys = keys[1];
            repeatMask = 0x40;
            goto repeat;
        }
    } else if (keys[0] & 0x80) {
        if (!(p->prevKeys & 0x80))
            p->repeatTimer = 0;
        timer = &p->repeatTimer;
        if (++*timer > 9) {
            pendingKeys = keys[1];
            repeatMask = 0x80;
        repeat:
            pendingKeys = repeatMask | pendingKeys;
            repeatMask = 0;
            keys[1] = pendingKeys;
            *timer = repeatMask;
        }
    }
    p->prevKeys = keys[0];
    if (keys[1] & 0x40) {
        Sound_PlaySong(0xDC);
        p->selection = p->selection > 0 ? p->selection - 1 : 3;
        SceneTextSelector_UpdatePanel(p);
    } else if (keys[1] & 0x80) {
        Sound_PlaySong(0xDC);
        p->selection = p->selection <= 2 ? p->selection + 1 : 0;
        SceneTextSelector_UpdatePanel(p);
    }
}
