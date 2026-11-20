"""Replay a recorded frame with a single OBJ slot hidden in the renderer."""
from pathlib import Path
import shutil
import tempfile
from uuid import uuid4

from . import fields, graphics, interventions, recipe, runner


def input_until(spec, trial, frame, actions):
    commands = recipe.timeline(recipe.compile_steps(trial["input"], spec["duration"]),
                               spec["duration"], spec["sample_every"], interventions=actions)
    for line in commands:
        action, argument = line.split(" ", 1)
        if action == "sample" and argument == f"f{frame:04d}":
            return
        if action in {"hold", "wait", "lux", "setfield"}:
            yield line
    raise ValueError("Choose a sampled frame")


def replay_image(binary, folder, seed, commands, rtc):
    from PIL import Image

    folder.mkdir()
    shutil.copyfile(seed, folder / "fork.ss")
    runner.run_harness(binary, folder, [*commands, "shot output", "save output"], rtc)
    with Image.open(folder / "output.ppm") as image:
        rgb = image.convert("RGB")
    state_id = runner.fingerprint(folder / "output.ss")
    return rgb, state_id


def store_card(path, baseline, hidden, position, slot):
    from PIL import Image, ImageDraw

    x, y = position
    card = Image.new("RGB", (760, 210), "#181b25")
    draw = ImageDraw.Draw(card)
    factor = 7
    radius = 12
    crop = (x - radius, y - radius, x + radius, y + radius)
    size = (radius * factor * 2,) * 2
    draw.text((10, 10), "Recorded frame", fill="#f4f2ed")
    card.paste(baseline, (10, 30))
    ix, iy = 10 + x, 30 + y
    draw.rectangle((ix - radius, iy - radius, ix + radius, iy + radius), outline="#ffe19d")
    for left, title, image in ((278, "Control replay", baseline), (476, f"Hardware OBJ {slot} hidden", hidden)):
        draw.text((left, 10), title, fill="#f4f2ed")
        top = 30
        card.paste(image.crop(crop).resize(size, Image.Resampling.NEAREST), (left, top))
        px, py = left + radius * factor, top + radius * factor
        draw.rectangle((px - 1, py - 1, px + factor, py + factor), outline="#ffe19d")
    card.save(path)


def execute(directory, result, trial_id, frame, x, y, slot):
    from PIL import Image, ImageChops

    evidence = graphics.inspect(directory, result, trial_id, frame, x, y, runner.ROM)
    slot = recipe.bounded_int(slot, 0, 127, "Hardware OBJ slot")
    if frame == 0 or not any(obj["slot"] == slot for obj in evidence["objects"]):
        raise ValueError("Choose a matching OBJ slot on a frame after the shared start")
    if runner.fingerprint(runner.ROM) != result["identity"]["rom"]:
        raise ValueError("The trial was recorded with a different ROM")
    seed = directory / "prepare/fork.ss"
    if runner.fingerprint(seed) != result["identity"]["fork"]:
        raise ValueError("The experiment's starting state changed on disk")
    spec = result["recipe"]
    trial = next(t for t in result["trials"] if t["id"] == trial_id)
    snap = next(p for p in trial["snapshots"] if p["relative"] == frame)
    with Image.open(directory / trial_id / f"{snap['label']}.png") as image:
        recorded = image.convert("RGB")

    rtc = spec["rtc"]
    setup = ["trace off", "load fork", f"lux {trial['snapshots'][0]['sensor']}", f"rtc {rtc}", graphics.trace_command()]
    observed = [fields.Field(**row) for row in result["fields"]] if trial.get("interventions") else []
    setup.extend(field.command() for field in observed)
    controller = list(input_until(spec, trial, frame, interventions.encoded(trial, observed)))
    suppress = f"hideobj {slot} {snap['frame']}"
    binary = runner.build_harness()
    with tempfile.TemporaryDirectory(prefix="pixel-", dir=runner.HARNESS.parent) as temporary:
        path = Path(temporary)
        baseline, controls = replay_image(binary, path / "baseline", seed, setup + controller, rtc)
        hidden, tested = replay_image(binary, path / "isolated", seed, setup + [suppress] + controller, rtc)
    if controls != snap["state_sha256"] or baseline.tobytes() != recorded.tobytes():
        raise RuntimeError("The control replay did not reproduce the recorded state and image")
    if tested != controls:
        raise RuntimeError("Suppressing the sprite changed the serialized emulator state")

    position = (x, y)
    before, after = baseline.getpixel(position), hidden.getpixel(position)
    identifier = uuid4().hex
    artifacts = directory / "pixel-proofs" / identifier
    artifacts.mkdir(parents=True)
    proof = {
        "id": identifier, "run": result["id"], "trial": trial_id, "frame": frame, "game_frame": snap["frame"],
        "coordinate": position, "slot": slot, "before": before, "after": after, "changed": before != after,
        "original_frame_reproduced": True, "matching_final_state_sha256": controls,
        "changed_bounds": ImageChops.difference(baseline, hidden).getbbox(),
        "harness_sha256": runner.fingerprint(binary),
        "contact_sheet": f"/api/experiments/runs/{result['id']}/artifact/pixel-proofs/{identifier}/contact.png",
    }
    store_card(artifacts / "contact.png", baseline, hidden, position, slot)
    runner.atomic_json(artifacts / "evidence.json", proof)
    return proof
