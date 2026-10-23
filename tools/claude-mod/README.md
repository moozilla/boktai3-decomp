# decomp-progress

A Claude Code mod that puts a small `◔ 64.50%` button in the prompt footer, beside
the mode labels. Clicking it (or typing `/decomp`) opens a popup with the Boktai 3
decomp's progress, in three tabs:

* **Overview**: the headline and one bar per category, by section. Hover the
  headline or a category for the details (bytes, register pinning, drafts, functions
  left); click a category to open it in the treemap. ROM regions fold at the bottom.
* **Treemap**: like decomp.dev's, one level at a time: categories, then a category's
  units, then a unit's functions, sized by code bytes and shaded by bytes matched
  (green is complete). Hover a tile for numbers. Under it, the tiles as a list, most
  left to match first: click a row to go in, and at the function level to open the
  function's C in a pane beside the chat, read from the session's checkout of this
  repository when it is one (else from `main` on GitHub).
* **Recent PRs**: merged PRs with the matches and progress each one reports.

It reads `PROGRESS.md` from `main`, the GitHub API's list of closed PRs, the
decomp.dev page and, once the treemap is opened, decomp.dev's full report,
refreshing every ten minutes (↻ refreshes now). Nothing is written anywhere but
the mod's own store.

## Install

At the prompt of a terminal session:

```
/plugin install decomp-progress --marketplace moozilla/boktai3-decomp
```

In the desktop app: click + next to the prompt box, then Plugins, Add marketplace
with `moozilla/boktai3-decomp`, then Add plugin.

## Developing

`claude plugin validate tools/claude-mod` and `claude plugin test tools/claude-mod`
check the manifest and run `tests/popup.test.tsx`. `claude --plugin-dir tools/claude-mod`
loads it from the checkout.
