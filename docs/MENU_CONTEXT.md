# Pause-menu config, sleep, and save context

The corrected `save_menus_field.txt` replay and its screenshots support three
small, tentative naming leads. The screenshot labels identify the stable tabs
as 操作設定 (operation settings), スリープ (sleep), and セーブ (save). The
coverage marks begin after each tab transition and include a short idle period
and directional navigation. In the corrected traces, each handler's entry
halfword is observed only in its corresponding stable tab, among the stable
menu marks and field mark:

| Address | Stable mark with observed entry | Static behavior | Tentative name |
|---|---|---|---|
| `08175D34` | `menu_config` | Tests bit 0 of the halfword at `gUnk_03005260 + 2`; on A, prepares the config interaction and installs `sub_08175D78` through `sub_08163EB8`. | `PauseMenu_Config_HandleConfirm` (medium) |
| `08176020` | `menu_sleep` | Tests the same A bit; on A, begins the sleep interaction and installs `sub_081760A4` through `sub_08163EB8`. | `PauseMenu_Sleep_HandleConfirm` (medium) |
| `08176224` | `menu_save` | Tests the same A bit; on A, begins the save interaction and installs `sub_0817627C` through `sub_08163EB8`. | `PauseMenu_Save_HandleConfirm` (medium) |

The names describe the screen and input path suggested by the Japanese labels
and static code. They are proposals, not proven user-visible semantics. In
particular, the replay visits these screens but does not press A while their
stable marks are active. The A branches, and the callback entries
`08175D78`, `081760A4`, and `0817627C`, are therefore not observed in this
replay. Confirm their behavior with a human-controlled or scripted A press on
each tab, recording the resulting prompt/state and whether cancel returns to
the same tab.

`sub_08163EB8` is a shared callback/state helper. Static inspection shows it
writes its callback argument at object offset `+0xA54`, clears a halfword at
`+0xA4E`, and stores a mode byte at `+0xA58`; the mode also selects which
input-related setup calls follow. Its entry was not observed in the three
stable tab marks or field mark, but was observed inside `menu_close`, where it
may be reached after a branch elsewhere in the close path. A tentative name is
`PauseMenu_SetCallback` (medium confidence for the callback/state role; the
precise meaning of its mode byte remains unknown).

These are execution-set observations, not call counts or proof of exclusive
ownership. They do not name the surrounding pause-menu architecture. Static
behavior was independently checked with `tools/asmat.py` at the four
addresses above. Scene labels were checked against
`build/context/verified-screens.png`; corrected per-mark traces are in
`build/context/stable-menu-field/`. The replay and coverage remain local,
ROM-derived evidence and are not committed.
