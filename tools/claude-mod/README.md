# decomp-progress

A Claude Code mod that puts a small `◔ 91.52%` button (code matched without concessions) in the prompt footer, beside
the mode labels. Clicking it (or typing `/decomp`) opens a popup with the Boktai 3
decomp's progress, in three tabs:

* **Overview**: the headline, the share of code matched without concessions (as decomp.dev
  counts it), with its bar (without concessions, and with them), then one bar per category. Click a category to open it in the treemap.
* **Treemap**: one level at a time, like decomp.dev: categories, then a category's
  units, then a unit's functions. Tiles are sized by bytes; click one to zoom in, and
  use the breadcrumb to go back up. At the function level, clicking a function opens
  its C source from main in a second pane (the sidebar), with a link to GitHub.
* **Recent PRs**: merged PRs with the matches and progress each one reports.

It reads `PROGRESS.md` and `PROGRESS.json` (the function tree, written beside it by
`tools/progress.py --markdown PROGRESS.md`) from `main`, and the GitHub API's list of
closed PRs, refreshing every ten minutes (↻ refreshes now). Mods load in Claude Code
sessions (the desktop app's Code tab and the terminal), not in project chats.

## Install

At the prompt of a terminal session:

```
/plugin install decomp-progress --marketplace moozilla/boktai3-decomp
```

In the desktop app: click + next to the prompt box, then Plugins, Add marketplace
with `moozilla/boktai3-decomp`, then Add plugin.

An installed copy updates only when `version` in `.claude-plugin/plugin.json` goes up, so
bump it with every change: `/plugin marketplace update boktai3-decomp`, then update the
plugin (or reinstall it).

## Developing

`claude plugin validate tools/claude-mod` and `claude plugin test tools/claude-mod`
check the manifest and run `tests/popup.test.tsx`. Run them with a `claude` as new as the
engine the mod runs in (the desktop app ships its own, newer than a stale terminal install). `claude --plugin-dir tools/claude-mod`
loads it from the checkout.
