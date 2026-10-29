# A matching search engine: spec and brief (2026-10-28)

Tags: #tooling #matching #compiler #process

This is the brief for the next generation of matching tooling. Workers building it should read all of
it, including the owner's words in Appendix A, which are the source of truth where this summary falls
short. It was written by the coordinator (Claude) after two subsystem runs, two adversarial reviews and
a long discussion with the owner.

**Be ambitious.** The owner believes this is buildable, and buildable fast, by agents like you, and that
the project and the field need it. Don't scale it down to a safe increment. Challenge the ideas here,
including the owner's, where you see a better design; say why, then build the better one. Don't get
fixated on reward hacking either: the ROM is an objective check, and a byte-identical match is real
progress. The goal behind everything is 100% of the game in matching C that reads like what the original
programmers wrote.

## 1. Where the project stands

* Code matched in C: 79.66% (11,438 of 11,798 functions; PROGRESS.md). Without register pinning:
  56.6%. Pins are a temporary concession the project allows; the end state has none.
* 360 NONMATCHING drafts remain: 490 KB, 20.3% of code. They average 1.36 KB, twice the size of what
  has matched. Monsters is 274 KB of it; 66 drafts are over 2 KB; the largest is 8.7 KB.
* The matching loop today: a coding model (Codex gpt-6.1-sol in subsystem runs) edits a draft's C in a
  text editor, compiles it through `tools/scope/scope.py` (instrumented agbcc, about 12 ms per variant,
  0.12 s per diff with all RTL dumps), reads the diff and scope's explanations, and tries again. Two
  runs attempted 390 functions and matched 93; the second matched 29 (14 pin-free) for about 45 points
  of the weekly Codex quota.
* Where workers stopped (last log line per open function, run 2): a scope knob reproduces the ROM but
  no C was found (`AGBCC_SPILL` reload rotation, `NOPRE`); a register missing from reload's spill set;
  big functions 60+ instructions off, where workers spent 2,000-18,000 compiles without converging.
  Every "only reload rotation" case was a pinned draft: removing the pin changes the shape. Logs:
  `~/git/wt/old-sc1/*/build/scope/campaign/LOG.md`, `~/git/wt/sc-r2-*/build/scope/campaign/LOG.md`, and
  `REPORT.md` / `best.c` / `solve.log` per function next to them.

## 2. What is wrong with the current approach

The owner's diagnosis (Appendix A, messages 3 and 6), which the coordinator agrees with:

* The tools explain one compile at a time. The search itself (what to try next, remembering what was
  tried, backing out of a dead end) is left to a model editing text, so it is trial and error with no
  memory but the current best file. That is why workers end in local maxima: pins that create shape,
  scaffolding locals, casts and views that score well and block the real struct or macro.
* The search tools that exist are blind: `tools/rasolve.py` (brute-force priority and rotation search
  after forcing registers; 26,149 compiles on sub_081AA788), the decomp-permuter (`tools/permute.py`,
  random AST mutations). That is spray and pray, the thing scope was meant to replace.
* Knowledge accreted as janky databases and many READMEs and notes (idioms.py, relatives.py,
  causebook.py, matching-tricks.md, agbccsrc.py, scope's README). A model is already a database of
  idioms; extra context should fill gaps in its training data only, and be short.
* Instructions to agents have been too strict. Give them the goal, the principles and the freedom to do
  what drives the project forward.
* The original vision was a stateful, IDE-like tool. scope.py got part of the way (a resident server,
  fast compiles, pass-by-pass explanations) and has reached diminishing returns.

## 3. The vision: constrained search, with the model as the policy

The owner's sketch (Appendix A, message 6), made concrete. Treat matching as search over source
programs:

1. **Solve what is determined first.** From the ROM and the compiler, establish everything the C cannot
   vary: calls and their order, memory accesses (base, offset, width, signedness), constants, the
   control-flow skeleton, which values exist and how long they live (gcc 2.95 keeps each pseudo in one
   hard register or stack slot for its whole life, so the ROM's def-use webs per register recover the
   original values; see build/review/opus.md section 3 and sol.md section 4). The result is the program
   laid out with its degrees of freedom made explicit.
2. **Make the degrees of freedom a move set.** Semantics-preserving transformations over the C syntax
   tree: introduce or remove a temporary, reorder independent statements, loop form, comparison
   direction, if/else order, goto vs early return, width or signedness where behavior is unchanged,
   struct field vs pointer arithmetic, extract or inline a helper, make a repeated shape a macro, change
   a declaration's scope. `tools/rewrites.py` already has a dozen of these as generators over the
   permuter's pycparser AST. A free-form edit by the model is also a move.
3. **A stateful session holds the search.** A tree (or graph) of variants: each node is a program, each
   edge a move, each node evaluated (bytes, per-region verdicts, which compiler decisions differ from
   the ROM and from the parent, semantic check). It keeps a frontier, so the model can branch, compare
   and back out, and it remembers what was tried across a session and across workers.
4. **The model is the policy.** The owner's analogy: a graph of all possible transformations, searched
   the way AlphaGo searches, with a policy choosing what to expand. We don't train a network: the LLM
   is the policy and the value estimate, the compiler and ROM are the environment, and evaluation costs
   milliseconds. The tool's job is to make each step cheap and informative: which decision changed,
   where in the compiler (file:line in the agbcc source scope runs, so the model can go and read it),
   which region is now exact.
5. **Decompose big functions.** They are made of components (loops, blocks, inline-helper bodies,
   switch arms). Solve components, then chain them and make the registers flow: shape is mostly local,
   allocation global. The session should let a worker work on one region while the rest stays fixed,
   with region verdicts that are not confounded by register shifts from earlier regions.

Open questions to answer by building, not by deliberating: how to represent the determined part (a
lifted value graph? constraints on the AST?); which moves matter and how to keep the set finite but
sufficient; how to score nodes so the search is not just descending the instruction diff (the owner's
local-maxima concern); how the model and the search algorithm share control (model proposes, tool
expands cheaply around it?); how the semantic check (`tools/semcheck.py`, differential execution in a
Thumb emulator) prunes wrong programs; how state persists across workers and sessions; whether reload
and allocation are better handled by search, by inverse reasoning from the ROM's assignment, or both.

## 4. What exists to build on (read it, reuse it, replace it if better)

* `tools/scope/`: `scope.py` (5,500 lines: diff, why, spill, pre, life, vs, try, shape, pins, probe),
  `fast.py` (fork-server compiles, relocation placement without ld), `agbcc.patch` (the instrumented
  agbcc: AGBCC_PRI, AGBCC_FORCE, AGBCC_SPILL, AGBCC_NOPRE, CSE_* and dump knobs), `README.md` (tool
  docs, "agbcc facts established with scope", gaps), `WORKER.md`, `campaign.py`, `land.py`.
* `tools/rewrites.py` (semantics-preserving AST rewrites), `tools/permute.py` (the permuter),
  `tools/semcheck.py` (differential execution), `tools/rasolve.py`, `tools/relatives.py`,
  `tools/idioms.py`, `tools/agbccsrc.py`, `tools/regdiag.py`, `tools/passdiff.py`.
* Compiler source: `build/scope/agbcc/gcc/` (scope's instrumented copy; reload1.c, global.c,
  local-alloc.c, gcse.c, cse.c, loop.c, combine.c, sched.c, flow.c, thumb.md).
* Knowledge: `docs/REGALLOC.md`, `docs/external/eds/matching-tricks.md` (the Yu-Gi-Oh! EDS project's
  catalog), `notes/arm-08000430.md`.
* Reviews of the previous plan: `build/review/opus.md` and `build/review/sol.md` (not committed; in the
  main checkout). Useful findings: rasolve's register inference votes by majority and hides
  contradictions; `force_allocno` does not check conflicts; scope's shape distance scores different
  dependencies the same; `spill_shifts` wrongly rejects shifting the first reload choice.

## 5. Principles

* The model writes the C; tools give it state, facts and fast feedback. Prefer pointing to the compiler
  source (file:line) over pasting it.
* Extra context only where training data has gaps: one short, symptom-indexed file of non-obvious agbcc
  facts beats a database.
* Fix the paper cuts and bugs you hit as you go. Replace a janky tool rather than layering on it; old
  tools can be retired.
* Measure by velocity on functions not yet matched: matches per hour and per token, how many need no
  pins, and how often workers hit paper cuts or grind. Matching the ROM is the objective test; no
  fixture suites of solved functions are needed.
* Project rules stand: byte-identical matching; no hand-written instructions in `asm()`; the ROM and
  anything generated from it are never committed; no personal information in commits (write `~`).

## 6. Tonight (2026-10-28)

The owner has about four hours and nearly a full weekly Codex quota, and wants the tool built in an
hour or two and then used hard by matching workers.

* **Two independent implementations**, then pick the best of both:
  * a Codex gpt-6.1-sol orchestrator, free to start Sol sub-agents (further `codex exec` sessions) for
    parts of the work and matching workers to try the tool on real drafts;
  * a Claude Opus builder, working alone or with its own sub-agents.
  Each in its own worktree and branch. Don't coordinate with the other implementation; do borrow from
  the existing tools.
* **GPT-6 Pro** gets a package with this spec and the relevant source, and may propose a third design.
* Then matching workers use the winner on the remaining drafts in ROM order, subsystem by subsystem,
  until the owner's bedtime (no overnight run).

## Appendix A: the owner's words, verbatim (2026-10-28)

Message 1:

> I have another codex reset that I need to use up tonight. I want to do something similar to what we
> did last time, but we didn't get that much matching with the amount of usage. Only like a 1% or a
> little bit less than 2% for about 50% usage. I'd like to try to drive higher than that. Like, could we
> get 10%? I think I have close to 100% usage left. I think we decided that GPT 6.1 Sol is probably
> still the best bang for our buck. There's always the option of using Astra, but it's very expensive,
> like 5x. I think what I'd like to do is continue working subsystem by subsystem and trying to finish
> stuff off. I'm not sure if it's possible to like evaluate some traces before we get started and try to
> like come up with a strategy to drive progress more, or if we're genuinely just on the hard part of
> the game. I'm a little bit skeptical of that. I feel like part of the issue is that the workers just
> keep giving up. I don't know. Can you like take a look, give me your thoughts, propose a plan before we
> get started? We have like, I don't know, until I go to bed, approximately 6 hours. Approximately 6
> hours until I go to bed.

Message 2 (after the coordinator proposed grouping near-identical "twin" functions):

> I'm a little bit worried that you're just searching for kind of reward hacked ways to make a cheap
> progress, because ultimately we need to do every function, remember? I want to find efficient ways to
> drive that without wasting too many tokens. Like the things that I'm considering are approaches like
> scope.py, where we're instrumenting the compiler more, like work along those lines, not just sorting
> stuff and allocating work differently, because that has not really proven to have paid off in the
> past, other than having workers work on the same system so they build up context. I think the twins
> are arguably that concept in some sense, but it feels a little bit reward hacked to me still.

Message 3 (on a plan for reload-rotation, spill-set/PRE and big-function tools):

> Let's start with tooling only and then we can scale up. A couple thoughts on my side. I think these are
> good ideas. A couple thoughts. One, I thought we had already done some of this. Like, I know you had
> messed around with reloads a bunch when we were first building scope.py. So potentially some of this
> already exists and this just needs to be more legible. I know one of our original ideas with the
> tooling was to give a reference in the GCC code so that the worker could read the relevant section of
> the compiler internals. I'm not sure if that's happening. Because one thing that's giving me a little
> bit of pause is, you know, the compiler can be forced to match, but we don't know the C, so we need to
> tell it which C change. That'd be defeating the purpose and tautological in a way, because the entire
> job of the workers is to generate the C based off of their knowledge as coding models, right? The job
> of the tooling is to give them all the context to make this efficient without having to do a bunch of
> trial and error. I think some trial and error is inevitable, considering going from internal
> representation to C is a little bit lossy. I'm still a little bit skeptical that that has to be the
> case, but I feel like that's an approach we've written off at this point, like this idea of going from,
> you know, ASM to RTL to C, something like that. Yeah, because I don't understand how you're going to
> show which C edit adds or removes the one reload. Unless they had like a slate of C edits they'd like
> to try and the tool could like try all of them and show what happened. It just feels like we need a
> little bit more of a way to do this intelligently. Another thing you mentioned was on the pre stuff,
> you know, you're saying show C forms that would stop it, including a trick. I'm a little bit hesitant
> when you say the word trick, because ultimately our goal is to avoid all tricks and come up with a
> realistic C that the programmers would have used. And I think in general, I keep seeing this problem
> that these reward hacked ways to match the code end up biting us later, where, you know, we're trying
> to drive down this gradient where we're matching more and more instructions and we get to like a local
> maxima. And, you know, some register pins in the simplest case, but it could be other stuff, prevent us
> from seeing the actual solution, which often is something like a struct or a macro, something that a
> human would actually use. So I almost feel like this idea of kind of trying to drive down that gradient
> isn't necessarily the right way to go. That said, I don't have a better suggestion. I feel like
> especially with the huge functions, to me, they feel like they're made up of subcomponents, like, you
> know, individual loops or blocks, and you kind of need to solve those on their own and then chain them
> together and figure out how to get the registers to flow through it correctly. This is my
> understanding. I don't have a good suggestion of how to do that intelligently, but it feels like what
> we've been doing is not quite, like if we knew the, if we had an oracle that could tell us the perfect
> way to do it, I feel like our approach is suboptimal. Thoughts?

Message 4:

> One other thought is potentially you could give a spin up like a maybe one more Opus worker and one
> more Soul worker, and kind of give them this prompt from me as well as all of my feedback in this vein,
> and call it like an adversarial plan and tooling review and get their feedback on the plan,
> emphasizing that we want to come up with like a way to not reward hack and a way to drive to 100%
> completion in the end.

Message 5:

> Can you explain the updated plan to me before you launch a bunch of workers? Okay to leave the ones
> started if you've already started them, but I'd like to review the plan myself.

Message 6 (on the plan the reviews produced: value webs, reload/PRE provenance, a trusted idiom corpus,
a frozen test panel):

> Yeah, I guess I appreciate this plan, but I'm quite skeptical of aspects of it, to be honest. Let me go
> through and give you my brain dump, and you can respond. You could also give it to the workers and see
> what their takes are as well. As for test functions being contaminated, I think, yeah, this is a
> problem, but I continue to be skeptical of the value of testing on functions we've already solved. I
> feel like we should be testing on net new functions, and like our tests should be continuous
> improvement towards the goal. Our measurement of if we are succeeding is our velocity of matching and,
> you know, rate of running into paper cuts, subjective toil. I think we need to be okay with this,
> because ultimately the problem is an objective problem that we can test by matching, you know,
> compiling the ROM, right? Right? Okay, point two. RA solve, I feel, is like an older tool that I feel
> like I don't trust very much as a process. It feels more of like this kind of spray and pray approach
> that I was trying to move away from when I first prompted for scope.py, and this sort of informed by
> compiler internals, some sort of tooling. Kind of same thing with idioms.py, and we have like a bunch
> of versions of this. There's also like various readmes and notes files everywhere. It feels
> inefficient, and the agents like have the ability to search that, as well as like manually search for
> other functions in the code base that have been successfully matched. It just feels incredibly
> rudimentary to me. I guess like certain idioms could be useful to know, but, and we also, for example,
> there's also the Yu-Gi-Oh! ESD article that we compiled, or that we copied from their wiki. I feel like
> we could create a really concise like piece of context that just goes to the workers with idioms, and
> it shouldn't be like thousands of lines. It should be a small number with the very obvious ones left
> out and the non-obvious ones included, and, you know, it's structured in such a way where the agents
> could load it piecewise based off of the thing that they're trying to solve or the thing that they're
> stuck on. I think the problem is agents won't know how to use it, and that itself is a pretty
> difficult prompting problem to solve. And this kind of is a broader principle that I think applies to
> a lot of the ideas in this plan and that you and other agents have proposed repeatedly, is this idea
> of like kind of sloughing off things that in my opinion are best suited for intelligence to kind of
> like a janky database of idioms. Like if you think about it or you squint your eyes, that's
> essentially what an LLM is already, right? So I'm skeptical of this approach in general. Like I think
> the purpose of our additional context or database of idioms is to fill in gaps in the training data
> only. Right, and on my ASM to RTL to C idea, I think there was a lot more detail in the actual plans. I
> briefly looked at what Souls was saying, I think, and you kind of smoothed out some of the detail
> there, but my understanding of how the compiler internals work is there's sort of like a cancellation
> phase, and you could, like it's only lossy insofar as there are transformations that are allowed, and
> potentially like infinitely many of them. And I guess the way that I see this being optimally solved
> is, like say you did have a solver. You would do something like solve the stuff that's fully
> deterministic and constrained first, so you have the program laid out in a way where all of the kind
> of degrees of freedom are obvious, and then you only allow the solver to vary those. And the way I
> would think about it is you would build a graph of all the possible transformations. You know, say you
> add like a plus one minus one here. This is just an example. I think it's kind of a bad one, but you
> know what I mean. And you can imagine like the graph kind of extends out infinitely based off all the
> various transformations. And you could do some sort of Monte Carlo search or some sort of AlphaGo-like
> neural policy search over it and kind of come up with an informed way to like brute force over the
> policy space. There may be some errors in my thinking there. It's a sloppy idea. I'm not saying we
> should build this per se, because I think that a process like that can be approximated with LLMs,
> right? Like we don't need to train a neural net. We can just put in an LLM in the process of where it
> would be driving the search. Does that make sense? And it seems like what we're doing is a very, very
> poor approximation of that currently. And the plan here is not ambitious. Like the idea of web, I think
> it's like somewhat in the right direction. I think we're overly focused on tests. And provenance. Like
> this idea of inlining 10 to 30 lines of the compiler source. I think it's more important to give it
> the line number and just encourage it to look. Like even your prompts to these other agents for the
> adversarial plan is very like, do only this, only read only. Like I want to stop giving agents these
> such strict instructions. I want to allow them to be agents, you know, and do what is best, what they
> think is best to drive the project forward, even if that is something a little bit creative and off
> the beaten path. Obviously, I think we should fix bugs as we go, but I think part of the problem with
> this project is this like accumulation of kind of janky tools earlier on, and not like not trying to
> build this more ambitious approach that's going to actually solve us there and drive the field
> forward, frankly. Like I think you and the other agents are smart enough and capable enough to like
> actually build the thing I'm talking about. You know, when I originally came up with this plan, I knew
> at the very start of this conversation, it's probably been compacted to oblivion at this point, but we
> were talking about like a stateful kind of IDE like tool, and what we ended up with was scope.py. And
> I think scope.py definitely helped a little bit, but has reached diminishing returns. So I want to
> rethink our approach a little bit. I do think that scope.py is an improvement over rasolve.py, for
> example, but can you see why I'm so disappointed with this, and like why I feel like we're not
> progressing in my vision? And then one last comment on the timeline. You know, it's not 6 hours now,
> because it's probably more like 4 or 5 hours. And it's like, I don't want to spend that long doing the
> tooling. I want to spend 1 or 2 hours doing the tooling and then drive the workers hard with the
> tooling with the remaining time I have. They could potentially even run overnight, but then I'd have
> to have an agent try to trigger the reset automatically, which I could do, but I would prefer to
> avoid.

Message 7:

> This seems directionally correct. In a plan, I feel like you still somehow hedged my ambition a little
> bit. I want to, like, have the prompts really emphasize that. Be ambitious. Challenge my ideas. Do not
> get over fixated on reward hacking. This is possible. It's not going to take hours. I think what I
> would like, I think your idea of having two sole workers building on, like, small chunks of this idea,
> I don't think it's the right approach. I think you could have one sole worker that orchestrates other
> sole workers, and you can say, feel free to start sub-agents. I'd like to also maybe have an Opus
> worker. So we can come up with two parallel implementations and then pick the best of both. We could
> potentially even have GPT-6 Pro take a pass. I think maybe before you start, write up everything that
> we've talked about, like a spec, and including all of my spitballs verbatim, in like a big doc. And I
> guess for GPT-6 Pro, a package with including all of the relevant, like, source code and so on, like
> we did last time. Does that sound good?

("Soul"/"sole" is speech-to-text for Sol; "ESD" is the Yu-Gi-Oh! EDS project.)
