#!/usr/bin/env python3
"""Check return declaration repair against real source and unchanged call sites."""
from pathlib import Path
import sys
ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'port/core'))
from returns import repair_returns, verify_returns

verify_returns(ROOT)
before = '''void Task_Kill(void *);
extern void Trigger_ConsumePending(void *, unsigned);
void untouched(void *);
void (*callback)(void *);
s32 Task_Kill(void *);
Task_Kill(argument);
'''
after = repair_returns(before)
assert after == '''s32 Task_Kill(void *);
extern u32 Trigger_ConsumePending(void *, unsigned);
void untouched(void *);
void (*callback)(void *);
s32 Task_Kill(void *);
Task_Kill(argument);
'''
assert repair_returns(after) == after
print('25 actual C return definitions verified; declaration repair is selective and idempotent')
