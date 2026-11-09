void ResultGame_UpdatePlayerMarker(u8 *p)
{
    int *velocity;
    u32 key = 1 & gUnknown_03005260[1];
    if (key) {
        velocity = (int *)(p + 0x90);
        *velocity = gUnk_0824DB38[*(u32 *)(p + 0x50)];
        Sound_PlaySong(0x3ec);
    } else {
        int *initial = (int *)(p + 0x90);
        int v = *initial;
        velocity = initial;
        if (v > 0) {
            *velocity = key;
            p[0xb2] = key;
        } else {
            u8 *counter = p + 0xb2;
            u8 *table = gUnk_0824DB64;
            u32 index = *(u32 *)(p + 0x50);
            if (*counter < table[index]) {
                *velocity = key;
                ++*counter;
            } else {
                *velocity = v - gUnk_0824DB6F[index];
                if (*velocity < -22)
                    *velocity = -22;
            }
        }
    }
    *(int *)(p + 0x8c) += *velocity;
}
