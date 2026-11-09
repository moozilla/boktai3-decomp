void StatusScreen_FadeAndExit(struct StatusScreen *p)
{
    int factor = 32 - p->timer, bank, i;
    int blueMask;
    u16 *buffer;
    bank = 0;
    *(&buffer) = p->buffer;
    blueMask = 31744;
    for (; bank < 2; bank++) {
        for (i = 0; i < 16; i++) {
            {
                u16 *out = &buffer[bank * 16 + i];
                int color = *out;
                int r = ((color & 31) * factor) & 0x7C0;
                int g = ((color & 992) * factor) & 0xF800;
                int blue = color & blueMask;
                {
                    int b = (blue * factor) & 0x1F0000;
                    *out = (r | g | b) >> 6;
                }
            }
        }
    }

    if (++p->timer > 31) {
        Bg_ClearTilemap(0);
        Script_Call(p->script, &p->args);
        Task_Kill(p);
    } else {
        p->dirty |= 15;
    }
}
