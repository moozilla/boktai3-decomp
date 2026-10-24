#!/usr/bin/env python3
"""The agbcc decisions that matching runs into: a short rule each, and the compiler source that makes it.

  agbccsrc.py                 list the topics
  agbccsrc.py TOPIC|WORDS     the rule and the source excerpt (file:line) for the best-matching topic

Each excerpt is located by a text anchor in build/agbcc-instr/gcc (made by
tools/agbcc-instr.sh), so it survives line shifts, and is kept to the lines
that hold the condition. tools/hint.py picks topics from the decision events
of a difference; this command is for asking directly ("cse skip", "biv zero",
"move2add"). Lines that mention agbcc_* are the instrumented fork's knobs, not
the original compiler.
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from romlib import ROOT  # noqa: E402

GCC = os.path.join(ROOT, "build", "agbcc-instr", "gcc")

# key: (keywords, rule, [(file, anchor, lines before, lines after)])
TOPICS = {
    "order": (
        "order schedule scheduler earlier later move sequence evaluation",
        "agbcc has no Thumb instruction scheduler: instructions come out in source evaluation order, minus what "
        "cse/gcse/loop/combine delete or hoist. An instruction the ROM has earlier or later is a value the "
        "original evaluated at another point: a local initialized there, a call or inline-helper argument "
        "(arguments are evaluated before the body), or a different statement order. One exception: values "
        "loop.c hoists. If the ROM loads a value among a loop's hoisted invariants (just before the loop, "
        "between other constants and addresses), the original computed it inside the loop body, at the "
        "matching point: loop.c moves invariants out in body order (see loop-invariant).",
        []),
    "cse-skip": (
        "cse skip skip-blocks branch around if block extended carry knowledge label",
        "cse extends a basic block across `if (c) x;` (path status AROUND) and keeps everything it knew before "
        "the `if` (values in registers, folded addresses), when: the branch's label has exactly one use "
        "(LABEL_NUSES == 1), the last real insn before the label is not a used label or a barrier, there is no "
        "label between the branch and its target, and the path has room (PATHLENGTH). The ROM recomputing or "
        "not folding a value after such an `if` means one of these failed for the original. `flip.py` checks "
        "with -fno-cse-skip-blocks.",
        [("cse.c", "/* If this is a conditional jump, we can follow it if -fcse-follow-jumps", 0, 22),
         ("cse.c", "/* Detect a branch around a block of code.  */", 0, 38)]),
    "cse-follow": (
        "cse follow jump follow-jumps barrier target",
        "cse follows a conditional jump into its target (path status TAKEN) when the target label is used only "
        "once and a barrier precedes it (the block before ends in a jump or return), carrying what it knew "
        "into the target block. rtlprobe.py follow finds the jump (CSE_NOFOLLOW=K). Values are shared only "
        "within one machine mode: a constant made in QImode by `(u8)(x & 7)` is not reused for an int "
        "`& 7` (spellings.sp_narrow_mask).",
        [("cse.c", "if (follow_jumps && q != 0 && GET_CODE (q) == BARRIER)", -2, 26)]),
    "dead store": (
        "dead store flow alias set strict aliasing deleted store mem_set_list type",
        "flow.c deletes a store as dead when, scanning a block backwards, a later store writes an rtx-equal "
        "address and no read in between may alias it (anti_dependence) and no insn in between sets the "
        "address register. agbcc -O2 turns on -fstrict-aliasing: accesses of different types are in "
        "different alias sets and never alias, so a void * field read between two u32 stores does not keep "
        "the first one. When the ROM keeps the store, the read between had the stored type "
        "(spellings.sp_alias_read `*(u32 *)&x->f`). An access directly through a union member, or through a char, is in alias set 0 and aliases everything (c_get_alias_set), so `((union { s16 v; } *)p)[i].v = x` keeps later loads of other types in place (sub_081D3C90: same effect as alias sets off). rtlprobe.py alias recompiles with alias sets off "
        "(AGBCC_NOSTRICT=1) to tell whether any difference is an alias decision.",
        [("flow.c", "this memory write is dead (remember", -8, 14),
         ("alias.c", "if (!writep && RTX_UNCHANGING_P (mem))", -12, 24)]),
    "cse-canon": (
        "cse canonical register class head copy make_regs_eqv outlives block longer lived replacement",
        "When cse records a copy `new = old` it puts NEW in OLD's equivalence class, and NEW becomes the "
        "class's canonical register (every later use of the value is rewritten to it) if NEW lives past "
        "the current extended block, or was set before it, and its last use is later than the current "
        "head's. So `v = t; v -= y;` with v used after a branch makes v canonical for t's value, and t's "
        "computation moves to the copy. Without the copy t keeps its pseudo and its place "
        "(spellings.sp_fold_copy: `v = t - y`; SceneActorControl_UpdateHeightReturn). An inline "
        "function's return value is such a copy too: `if (!p) return 0; return x & m;` sets the return "
        "pseudo before the block that copies x & m into it (sub_08141C40, sub_08143ED0, still open). "
        "rtlprobe.py canon tries CSE_NOCANON=N per class head; fix.py runs it on near matches and then "
        "tries this decision's spellings first.",
        [("cse.c", "  /* Prefer fixed hard registers to anything.  Prefer pseudo regs to other", 0, 24)]),
    "reload": (
        "reload spill rotation reload register choose last_spill_reg ldrsh zero index scratch constant offset",
        "Reload picks a register for a constant operand, a spilled pseudo or an insn's scratch (the zero "
        "index of `ldrsh rD, [rB, rI]`) by rotation (reload1.c allocate_reload_reg): it starts after the "
        "register the previous reload took and takes the first spill register that no live pseudo holds "
        "and no other reload of the insn took. The spill registers are those some insn needing a reload "
        "chose (order_regs_for_reload: free call-clobbered r0-r3 first, lowest number first), over the "
        "whole function. tools/reloaddbg.py prints, for each reload register that differs from the ROM, "
        "the rotation state and the pseudo that held a skipped register; tools/rasolve.py searches rotation "
        "shifts. Value-before-address order of a far store is an inline setter (spellings.sp_outline_setter).",
        [("reload1.c", "/* I is the index in spill_regs.", 0, 30),
         ("reload1.c", "  /* Prefer registers not so far used, for use in temporary loading.", 0, 16)]),
    "reload-mode": (
        "reload inheritance find_equiv_reg mode copy back high register mov ip r0 HImode SImode reload_cse no-op move",
        "A value set into a high register through a low one (`movs r0, #7; mov ip, r0`) and used at once is "
        "normally read from the low register: reload inherits it (choose_reload_regs, find_equiv_reg), or "
        "reload_cse_regs or jump's no-op move deletion (jump.c) drops the copy back. All three compare modes "
        "(find_equiv_reg: \"the copy-insn moved the wrong sort of datum\"), so the ROM's `mov ip, r0; mov r0, "
        "ip` means the set and the use differ in mode: an HImode constant pseudo (`x & 7` narrowed to a "
        "16-bit field) used through a paradoxical SImode subreg. It survives cse only when the constant is a "
        "literal and the other operand is a word (an inline helper returning s32, not u16): "
        "spellings.sp_helper_ret_word, sp_const_literal (ActorParticles_SpawnWideBurst; the matched "
        "sub_08239D38 has the same helper). fix.py detects the copy and tries these first; the instrumented "
        "compiler's AGBCC_NOINHERIT=UID, AGBCC_NORCSE=1 and AGBCC_NOEQUIVMOVE=all turn the three off.",
        [("reload.c", "  /* Reject VALUE if the copy-insn moved the wrong sort of datum.  */", 0, 3),
         ("jump.c", "	  /* Detect and ignore no-op move instructions", 0, 20)]),
    "inline-result": (
        "inline return value target result argument same register call-saved integrate shares register",
        "An inlined call's return value is stored straight into the target of the assignment it feeds "
        "(integrate.c: map->inline_target), not into a pseudo of its own. An argument that overlaps the "
        "target is first copied to a fresh pseudo, but cse sees the variable still holds that value and "
        "rewrites the copy's uses back to it. So `x = H(x, ...)` makes x, H's parameter and H's result one "
        "pseudo: it lives as long as the result (crossing its calls, so a call-saved register) and the "
        "inline body keeps its own operand order. The ROM's sign: an argument and the value computed from "
        "it in one register (sub_08137E28: x and the cell index both r4). spellings.sp_result_into_arg; "
        "rtlprobe.py try-env AGBCC_PREF=N:R confirms it as a preference.",
        [("integrate.c", "		  && reg_overlap_mentioned_p (arg_vals[i], target))", -4, 8),
         ("integrate.c", "      if (REG_FUNCTION_VALUE_P (loc))", -2, 5)]),
    "cross-jump": (
        "cross jump tail merge identical branch ends shared tail copy alias parameter entry register",
        "jump.c's cross-jumping merges two code paths that end in identical insns before jumping to the same "
        "place: one path's tail is deleted and it jumps into the other's. Any register that differs keeps the "
        "tails apart. A copy of a parameter made at entry (`T *self = p;`) is replaced by p only inside the "
        "extended block where cse saw the copy, so uses of self after a label stay on their own pseudo and "
        "register. The ROM's sign: two branch tails identical except for one base register (`adds r1, r4` / "
        "`adds r1, r7`) and an entry copy `adds r7, r4, #0` (Slot7_UpdateMotionPhase). "
        "spellings.sp_param_alias.",
        [("jump.c", "/* Compare the instructions before insn E1 with those before E2", 0, 13)]),
    "cse-reuse": (
        "cse reuse deleted same value common subexpression recompute equivalent",
        "cse deletes a computation when the same value is already in a register within the extended basic "
        "block (a block plus the jumps it follows or skips). To make agbcc recompute it as the ROM does, the "
        "expression must differ (another type, base, or offset spelling), or the block must end before it "
        "(see cse-skip, cse-follow). A constant is reused only by an expression of the same machine mode.",
        [("cse.c", "/* Find the end of INSN's basic block and return its range,", 0, 13)]),
    "cse-swap": (
        "swap operand order commutative constant second canonical copy two-operand and orr",
        "fold_rtx puts an operand it knows to be a constant (or a register cse knows holds one) second in a "
        "commutative operation. For two-operand Thumb `ands`/`orrs`/`adds`, regmove then copies operand 1, so "
        "this order decides which register is copied. If the ROM copies the constant-holding register, its cse "
        "did not know that value was constant here.",
        [("cse.c", "/* If a commutative operation, place a constant integer as the second", 0, 18)]),
    "regmove-copy": (
        "regmove copy two-operand operand 1 adds #0 mov",
        "Thumb arithmetic is two-operand: the output must be operand 1's register. When operand 1 lives on after "
        "the insn, regmove (or reload) copies it first (`adds rD, rX, #0`). Which operand is operand 1 was "
        "fixed earlier (see cse-swap); whether a copy is needed depends on operand 1 dying here.",
        [("regmove.c", "/* Now scan through the operands looking for a source operand", 0, 22)]),
    "regmove-tie": (
        "regmove tie tied same register output input match",
        "regmove gives an insn's output the register of the input it must match, by rewriting the insn that "
        "set that input, if the input dies here and nothing in between uses either.",
        [("regmove.c", "/* Now scan through the operands looking for a destination operand", 0, 20)]),
    "biv-final": (
        "biv final value dead induction variable loop zero after loop counter eliminated",
        "When loop.c eliminates an induction variable that is dead after the loop, final_biv_value can still "
        "emit its final value (often `reg = 0`) after the loop, and cse2 then reuses that register for a later "
        "zero. A zero that sits at the loop exit in the ROM comes from this.",
        [("unroll.c", "/* Try to calculate the final value of the biv, the value it will have at", 0, 70)]),
    "loop-invariant": (
        "loop invariant hoist movable moved out of loop before loop",
        "loop.c moves an invariant computation out of the loop when it is set once in the loop and its "
        "operands do not change there (scan_loop decides; move_movables moves). A value the ROM computes "
        "inside the loop was not invariant, or was too cheap to move.",
        [("loop.c", "/* Scan MOVABLES, and move the insns that deserve to be moved.", 0, 4)]),
    "strength-reduce": (
        "strength reduce reduction induction giv biv pointer increment loop counter",
        "loop.c turns `base + i*size` inside a loop into a pointer incremented each pass (a giv), and can "
        "eliminate the counter. The ROM's loop variables tell which spelling the original used: a pointer "
        "walked by hand, an index, or both.",
        [("loop.c", "/* Perform strength reduction and induction variable elimination.", 0, 14)]),
    "gcse": (
        "gcse pre partial redundancy global copy propagation across blocks",
        "gcse moves or reuses a value computed in an earlier block (partial redundancy elimination) and "
        "propagates copies and constants across blocks.",
        [("gcse.c", "/* Top level routine to perform one PRE GCSE pass.", 0, 4)]),
    "combine": (
        "combine merge insns canonical complex operand",
        "combine merges up to three dependent insns into one when the result is a valid instruction, and "
        "canonicalizes commutative operands (complex ones first) when it rebuilds an insn.",
        [("combine.c", "/* Try to combine the insns I1 and I2 into I3.", 0, 13)]),
    "local-alloc": (
        "local alloc register priority within block quantity qty refs lifetime",
        "local-alloc gives registers to pseudos that live in one block, in order of priority "
        "floor_log2(refs) * refs * size / lifetime: more uses and a shorter life go first and get the "
        "lower-numbered free register. Declaration order breaks ties.",
        [("local-alloc.c", "#define QTY_CMP_PRI_BASE(q)", -10, 6)]),
    "global-alloc": (
        "global alloc register priority across blocks allocno live length",
        "global-alloc colours pseudos that cross blocks, in order of floor_log2(refs) * refs / live length "
        "(times size); ties go to the lower pseudo number, so declaration and first-use order break ties.",
        [("global.c", "allocno_compare (v1p, v2p)", -3, 26)]),
    "move2add": (
        "move2add reload constant derived adds subs movs register still holds",
        "After reload, reload_cse_move2add turns `movs rX, #K` into `adds/subs rX, #d` when rX still holds K-d "
        "from an earlier constant, with no label in between. Whether that happens depends on which register "
        "reload picked earlier (see reload).",
        [("reload1.c", "/* Try to transform (set (REGX) (CONST_INT A))", 0, 30)]),
}


def excerpt(fname, anchor, before, after):
    """(path:line, [lines]) of the anchored excerpt, or None."""
    path = os.path.join(GCC, fname)
    if not os.path.exists(path):
        return None
    lines = open(path, errors="replace").read().splitlines()
    at = next((i for i, ln in enumerate(lines) if anchor in ln), None)
    if at is None:
        return None
    lo, hi = max(0, at + before), min(len(lines), at + after + 1)
    return f"{os.path.relpath(path, ROOT)}:{at + 1}", [(n + 1, lines[n].rstrip()) for n in range(lo, hi)]


def show(key, source=True):
    words, rule, srcs = TOPICS[key]
    out = [f"-- {key}: {rule}"]
    if not source:
        return out
    for fname, anchor, before, after in srcs:
        ex = excerpt(fname, anchor, before, after)
        if ex is None:
            out.append(f"   ({fname}: not found; run tools/agbcc-instr.sh)")
            continue
        where, body = ex
        out.append(f"   {where}")
        out += [f"   {n:5} {t.expandtabs(8)}" for n, t in body if t.strip()]
    return out


def search(text):
    """Topic keys for free text, best first."""
    q = set(re.findall(r"[a-z0-9]+", text.lower().replace("-", " ")))
    if text in TOPICS:
        return [text]
    scored = []
    for k, (words, rule, _) in TOPICS.items():
        ws = set(words.split()) | set(k.split("-"))
        s = 3 * len(q & ws) + len(q & set(re.findall(r"[a-z0-9]+", rule.lower())))
        if s:
            scored.append((-s, k))
    return [k for _, k in sorted(scored)]


def main():
    if len(sys.argv) < 2:
        for k, (words, rule, _) in TOPICS.items():
            print(f"{k:16} {rule[:100]}...")
        return 0
    keys = search(" ".join(sys.argv[1:]))
    if not keys:
        print("no topic matches; topics: " + ", ".join(TOPICS))
        return 1
    print("\n".join(show(keys[0])))
    if keys[1:]:
        print(f"\nalso: {', '.join(keys[1:4])}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
