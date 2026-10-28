#include "background_flash.h"
#include "player.h"
void PlayerContact_ApplySyntheticContact(void *, void *, u32, u32, u32, u32);
void BackgroundFlash_EmitEffect(void *source)
{
    if ((*(void **)gPlayers))
        PlayerContact_ApplySyntheticContact((*(void **)gPlayers), source, 0, 0, 0, 0x1000);
}
