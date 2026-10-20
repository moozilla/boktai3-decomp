# sub_0824261C (Player update by kind) — unfinished

Draft reached 85% fuzzy under `check.py` (candidate-length) but only 57% with `--nonmatching`
(reviewed extent), so it was not committed. Structure recovered: kinds 1/8/2 in that source
order (matches the original block layout), kind 1 = charge accumulate, input mode
(sub_082420D4/sub_082422D8/sub_0823B728), table 08E87A10 callback, terrain step check
(same PackedVec {u32 x:16,y:16,z:16,pad:16} trick as sub_08242120/sub_08241504: a plain
local struct with a trailing pad bitfield makes agbcc RMW z instead of strh).
Remaining diffs: `v` for Player_AccumulateActionCharge is computed in r0 then moved to r1 (the
ldrb must not target r1), and the table index keeps `mode` live (lsls into a fresh reg).
