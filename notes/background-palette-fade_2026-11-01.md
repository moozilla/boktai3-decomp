# Background palette fade

Tags: 08217960 Palette_ApplyBackgroundFade

Palette_ApplyBackgroundFade matches all 604 bytes without register pins. The
nonzero-color path uses the object fade's packed RGB15 calculation. The old
draft narrowed the green mask and the weighted red/blue contribution to a
byte. Both values are words in the ROM. After computing the fade level,
the same working integer indexes the sixteen background banks.
