/* Tentative family: four pooled sprite/collider entries with health, hit feedback,
 * and a script callback. Unknown bytes retain their observed offsets. */
struct CallbackPoolEntry {
    u8 unknown0[0x1c];
    struct CallbackPoolPosition { s16 x, y, z, unknown6; } position;
    u8 unknown24[0x44-0x24];
    u16 sprite44;
    u8 unknown46[0x96-0x46];
    u16 damage;
    u8 unknown98[4];
    u16 hitFlags;
    u8 unknown9e[0x118-0x9e];
    struct CallbackPoolPosition savedPosition;
    u8 unknown120[0x13c-0x120];
    u32 spawnScript;
    void *healthDisplay;
    u8 unknown144[2];
    u8 spawnIndex;
    u8 unknown147[3];
    s16 health;
    s16 maxHealth;
    u8 unknown14e[3];
    u8 displayMode;
    u16 callbackIndex;
    u8 unknown154[4];
    s8 effectIndex;
    u8 unknown159[2];
    u8 hitTimer;
    u8 unknown15c[2];
    u8 defeated;
    u8 shakeTimer;
    u8 unknown160[0x24];
};
struct CallbackPool {
    u8 unknown0[0x1c];
    struct CallbackPoolEntry entries[4];
    u32 activeMask;
};
