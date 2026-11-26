#!/usr/bin/env python3
"""Preserve lane edits and synchronize untouched files during layout replay."""
import argparse
import csv
import hashlib
import io
import json
import os
from pathlib import Path
import stat
import subprocess

import reorg


TABLE_KEYS = {
    "source_dirs": ("current_path",),
    "header_dirs": ("current_path",),
    "source_files": ("original_path",),
    "function_renames": ("addr",),
    "prefix_renames": ("old_prefix", "scope"),
    "type_renames": ("old_name", "scope"),
    "function_aliases": ("addr",),
    "identity_corrections": ("current_path",),
}
TABLES = {Path("symbols", name + ".csv"): keys for name, keys in TABLE_KEYS.items()}
FUNCTIONS = Path("symbols/functions.csv")
TOOLS = tuple(Path("tools", tool) for tool in
              ("reorg.py", "source_layout.py", "split.py", "promote_names.py", "test_all.py", "replay_support.py"))
LOCAL_FILES = {Path("PROGRESS.md"), Path("PROGRESS.json"), Path("baserom.gba")}


def git(repo, *args):
    return subprocess.check_output(["git", "-C", str(repo), *args])


def nul_fields(raw):
    return [os.fsdecode(item) for item in raw.split(b"\0") if item]


def revision(repo, ref):
    return git(repo, "rev-parse", "--verify", "--end-of-options", ref + "^{commit}").decode().strip()


def tree(repo, ref):
    entries = {}
    for line in nul_fields(git(repo, "ls-tree", "-r", "-z", "--full-tree", ref)):
        info, name = line.split("\t", 1)
        mode, kind, oid = info.split()
        if kind == "blob":
            entries[Path(name)] = (mode, oid)
    return entries


def snapshot_csv(data, keys):
    if not data:
        return [], {}
    reader = csv.DictReader(io.StringIO(data.decode("utf-8"), newline=""))
    fields = reader.fieldnames
    rows = {}
    for row in reader:
        if extra := row.pop(None, ()):
            row[fields[-1]] = ",".join([row[fields[-1]], *extra])
        values = {field: row[field] or "" for field in fields}
        key = tuple(values[column].rstrip("/") if column == "scope" else values[column] for column in keys)
        if key in rows:
            raise ValueError(f"duplicate table key: {key}")
        rows[key] = values
    return fields, rows


def csv_bytes(fields, rows):
    output = io.StringIO(newline="")
    writer = csv.DictWriter(output, fields, lineterminator="\n")
    writer.writeheader()
    writer.writerows({field: row.get(field, "") for field in fields} for row in rows)
    return output.getvalue().encode()


def with_lane_additions(base, lane, reviewed, keys):
    reviewed_fields, result = snapshot_csv(reviewed, keys)
    lane_fields, local = snapshot_csv(lane, keys)
    _, original = snapshot_csv(base, keys)
    fields = list(dict.fromkeys(reviewed_fields + lane_fields))
    added = 0
    for key, row in local.items():
        if key in original:
            continue
        if key in result and any(result[key].get(field, "") != row.get(field, "") for field in fields):
            raise ValueError(f"both branches added different rows for {key}")
        if key not in result:
            result[key] = row
            added += 1
    return (csv_bytes(fields, result.values()) if added else reviewed), added


def read_local(path):
    return path.read_bytes() if path.is_file() else b""


def file_mode(path):
    if path.is_symlink():
        return "120000"
    if path.is_file():
        return "100755" if path.stat().st_mode & stat.S_IXUSR else "100644"
    return None


def file_bytes(path, mode):
    return os.fsencode(os.readlink(path)) if mode == "120000" else path.read_bytes()


def matches_blob(path, mode, oid):
    if file_mode(path) != mode:
        return False
    data = file_bytes(path, mode)
    algorithm = "sha1" if len(oid) == 40 else "sha256"
    digest = hashlib.new(algorithm, f"blob {len(data)}\0".encode() + data).hexdigest()
    return digest == oid


def install(path, mode, data=None):
    if not path.parent.resolve().is_relative_to(Path.cwd()):
        raise ValueError(f"{path}: parent leaves the replay checkout")
    if path.is_symlink():
        path.unlink()
    if not mode:
        path.unlink(missing_ok=True)
        return
    path.parent.mkdir(parents=True, exist_ok=True)
    if mode == "120000":
        path.unlink(missing_ok=True)
        path.symlink_to(os.fsdecode(data))
    else:
        path.write_bytes(data)
        path.chmod(0o755 if mode == "100755" else 0o644)


def blob(repo, entry):
    return git(repo, "cat-file", "blob", entry[1]) if entry else b""


def lane_paths(reviewed, fork, branch, exported):
    if exported:
        changed = git(reviewed, "diff", "--no-renames", "--name-only", "-z", fork, branch, "--")
        return set(map(Path, nul_fields(changed)))
    if Path(os.fsdecode(git(".", "rev-parse", "--show-toplevel")).strip()).resolve() != Path.cwd():
        raise ValueError("run replay from the branch checkout root")
    changed = git(".", "diff", "--no-renames", "--name-only", "-z", fork, branch, "--")
    changed += git(".", "diff", "--no-renames", "--name-only", "-z", branch, "--")
    changed += git(".", "diff", "--cached", "--no-renames", "--name-only", "-z", branch, "--")
    untracked = git(".", "ls-files", "--others", "--exclude-standard", "-z")
    return set(map(Path, nul_fields(changed + untracked)))


def prepare(args):
    reviewed = args.reviewed.resolve()
    exported = bool(os.environ.get("BOKTAI3_REPLAY_PATHS"))
    if exported != bool(args.branch_ref):
        raise ValueError("an archive requires BOKTAI3_REPLAY_PATHS and --branch-ref together")
    review = revision(reviewed, "HEAD")
    branch = revision(reviewed if exported else ".", args.branch_ref or "HEAD")
    fork = revision(reviewed, args.fork) if args.fork else git(reviewed, "merge-base", branch, review).decode().strip()
    for ref in (branch, review):
        git(reviewed, "merge-base", "--is-ancestor", fork, ref)
    protected = lane_paths(reviewed, fork, branch, exported)
    base = tree(reviewed, fork)
    reviewed_tree = tree(reviewed, review)
    merged = {}
    for path, keys in TABLES.items():
        if path not in reviewed_tree:
            raise ValueError(f"reviewed checkout is missing {path}")
        data, added = with_lane_additions(blob(reviewed, base.get(path)), read_local(path),
                                          blob(reviewed, reviewed_tree[path]), keys)
        if added:
            print(f"preserved {added} branch-added rows in {path}")
        merged[path] = data
    _, old_symbols = snapshot_csv(blob(reviewed, base.get(FUNCTIONS)), ("addr",))
    _, lane_symbols = snapshot_csv(FUNCTIONS.read_bytes(), ("addr",))
    branch_symbols = old_symbols if exported else snapshot_csv(
        git(reviewed, "show", f"{branch}:{FUNCTIONS}"), ("addr",))[1]
    changed_symbols = {key[0] for key in old_symbols.keys() | lane_symbols.keys() | branch_symbols.keys()
                       if not (old_symbols.get(key) == lane_symbols.get(key) == branch_symbols.get(key))}
    saved_tools = {}
    for index, path in enumerate(TOOLS):
        if path in protected:
            mode = file_mode(path)
            saved_tools[str(path)] = (mode, index)
            if mode:
                (args.state / str(index)).write_bytes(file_bytes(path, mode))
        if path not in reviewed_tree:
            raise ValueError(f"reviewed checkout is missing {path}")
    state = dict(reviewed=str(reviewed), ref=review, fork=fork, branch=branch,
                 protected=sorted(map(str, protected)), symbols=sorted(changed_symbols), tools=saved_tools)
    (args.state / "replay.json").write_text(json.dumps(state))
    for path, data in merged.items():
        path.write_bytes(data)
    for path in TOOLS:
        mode, _ = reviewed_tree[path]
        install(path, mode, blob(reviewed, reviewed_tree[path]))
    print(f"replaying {branch[:12]} from fork {fork[:12]} against {review[:12]} "
          f"with {len(protected)} branch-owned paths")


def protect_destinations(protected, placements):
    return protected | {placements[path] for path in protected if path in placements}


def place(args):
    state_file = args.state / "replay.json"
    state = json.loads(state_file.read_text())

    def remember(placements):
        protected = protect_destinations(set(map(Path, state["protected"])), placements)
        state["protected"] = sorted(map(str, protected))
        state_file.write_text(json.dumps(state))

    reorg.migrate(apply=True, remember=remember)


def sync_files(repo, paths, reviewed_tree):
    changes = [(path, reviewed_tree.get(path)) for path in sorted(paths)]
    if not changes:
        return 0
    count = 0
    # Read differing blobs through one Git process. Keep only one blob in memory at a time.
    with subprocess.Popen(["git", "-C", str(repo), "cat-file", "--batch"],
                          stdin=subprocess.PIPE, stdout=subprocess.PIPE) as process:
        for path, entry in changes:
            if entry is None:
                if file_mode(path):
                    install(path, None)
                    count += 1
                continue
            mode, oid = entry
            if matches_blob(path, mode, oid):
                continue
            process.stdin.write(f"{oid}\n".encode())
            process.stdin.flush()
            header = process.stdout.readline().split()
            if len(header) != 3 or header[:2] != [oid.encode(), b"blob"]:
                raise ValueError(f"{path}: Git did not return blob {oid}")
            size = int(header[2])
            data = process.stdout.read(size)
            if len(data) != size or process.stdout.read(1) != b"\n":
                raise ValueError(f"{path}: incomplete Git blob {oid}")
            install(path, mode, data)
            count += 1
        process.stdin.close()
        if process.wait():
            raise ValueError("git cat-file --batch failed")
    return count


def sync(args):
    state = json.loads((args.state / "replay.json").read_text())
    reviewed = state["reviewed"]
    original_tree = tree(reviewed, state["fork"])
    reviewed_tree = tree(reviewed, state["ref"])
    branch_tree = tree(reviewed, state["branch"])
    paths = original_tree.keys() | branch_tree.keys() | reviewed_tree.keys()
    paths.update(path.relative_to(Path.cwd()) for path in reorg.source_paths() + reorg.header_paths())
    protected = protect_destinations(set(map(Path, state["protected"])), reorg.recorded_moves())
    paths -= protected | TABLES.keys() | LOCAL_FILES | {FUNCTIONS}
    paths = {path for path in paths if path.parts[0] not in ("build", "gen")}
    changed = sync_files(reviewed, paths, reviewed_tree)
    for rel, (mode, index) in state["tools"].items():
        install(Path(rel), mode, (args.state / str(index)).read_bytes() if mode else None)

    keys = ("addr",)
    fields, symbols = snapshot_csv(blob(reviewed, reviewed_tree[FUNCTIONS]), keys)
    _, lane = snapshot_csv(FUNCTIONS.read_bytes(), keys)
    for addr in state["symbols"]:
        key = (addr,)
        if key in lane:
            symbols[key] = lane[key]
        else:
            symbols.pop(key, None)
    FUNCTIONS.write_bytes(csv_bytes(fields, (symbols[key] for key in sorted(symbols))))
    print(f"synchronized {changed} branch-untouched paths to {state['ref'][:12]}")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    commands = parser.add_subparsers(dest="command", required=True)
    start = commands.add_parser("prepare")
    start.add_argument("reviewed", type=Path)
    start.add_argument("state", type=Path)
    start.add_argument("--fork")
    start.add_argument("--branch-ref")
    for command in ("place", "sync"):
        commands.add_parser(command).add_argument("state", type=Path)
    args = parser.parse_args()
    try:
        {"prepare": prepare, "place": place, "sync": sync}[args.command](args)
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        parser.exit(1, f"layout replay: {error}\n")


if __name__ == "__main__":
    main()
