# Sound-test song lists

Tags: #sound #studio #data #matching #round8

SoundTest_CountEntries scans three signed halfword lists. A cursor, count and current entry let stock agbcc reproduce all 140 bytes without pinning the count to r2.

| Menu | First address | Songs |
|---|---|---:|
| Music | 086136F0 | 94 |
| Effects | 086137AE | 522 |
| Voices | 08613BC4 | 150 |

Each list ends with -1. The three ranges round-trip 1,538 bytes. The menu has 766 positions for 757 distinct songs. Song 7 is Music 81 and Effects 417. Song 227 appears at Effects 28 and 29. Effects 180 selects the zero-track song 963. Studio preserves the menu ordering and displays all positions on each song. Editing the extracted CSVs changes both the build and the Studio index.
