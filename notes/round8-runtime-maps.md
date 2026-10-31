# Runtime state and effect lookup tables

Tags: #ram #data #studio #matching #round8

The effect random bank, Q12 sine wave and arctangent lookup occupy 2,945 bytes. All three encode back to the ROM, and Studio links each table to its consumers. The random bank is editable after extraction. The two trigonometric CSVs can also be generated without a ROM.

| Start | End (exclusive) | Encoding |
|---|---|---|
| 086141BC | 086149BC | 1,024 unsigned halfwords, read by advancing the cursor at 03005304 modulo 1,024 |
| 086149C4 | 08614C44 | 320 signed halfwords, truncating 4096 times the sine of a 256-step phase |
| 08614C44 | 08614D45 | 257 bytes, truncating atan(i / 256) times 128 / π |

The sine table repeats its first 64 entries after a full turn. The interior label at 08614A84 starts at phase 96. The arctangent includes the equal-components case at ratio 256. Two adjacent four-byte groups in the angle data happen to resemble ROM pointers and must retain their numeric values when the data moves.

Random_InitValueTable fills the writable halfword bank at 0203B400 before independently initializing the RAM and ROM sample cursors. Random_Next15 updates a word at 03005278. Wireless input publishing uses the same four bytes for entries 2 and 3 of its five halfword slots. The RAM inventory records the word as an interior view.

Four 32-bit player status words start at 02000560. Player setup clears them by slot, link player creation writes the player kind, and nearby-player selection checks bit 3. The serial receive buffer contains 64 bytes, the MP2K instruction copy contains 1,024 bytes, and Wireless_InitRfuApi supplies an exact 0xE64-byte workspace to the RFU SDK. The known-size RAM inventory has a shared declaration for every byte. The size of gGameStateStorage remains open.
