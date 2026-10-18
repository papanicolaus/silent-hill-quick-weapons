"""Pure deterministic model tests; not a test of the Windows game/plugin ABI."""
import re
from pathlib import Path

source = (Path(__file__).resolve().parents[1] / 'src' / 'quick_weapons.c').read_text()
section = source.split('static const u8 kWeaponOrder[] = {', 1)[1].split('};', 1)[0]
order = re.findall(r'InvItemId_\w+', section)
assert len(order) == 11 and len(set(order)) == 11

section2 = source.split('static const QwBind kBinds[] = {', 1)[1].split('};', 1)[0]
binds = re.findall(r'\{ SDL_SCANCODE_(\w+), (InvItemId_\w+) \}', section2)
assert len(binds) == 11
assert len(set(k for k, _ in binds)) == 11
assert {v for _,v in binds} == set(order)

# The Qw_AdjacentOwned algorithm is mirrored here using arbitrary held item sets.
def adjacent(direction, current, owned):
    start = order.index(current) if current in order else (-1 if direction > 0 else 0)
    for j in range(1, len(order)+1):
        index = (start + direction*j + len(order)*2) % len(order)
        if order[index] in owned:
            return order[index]
    return current

for i in range(len(order)):
    for j in range(1, len(order)):
        owned = {order[i], order[j]}
        if len(owned) == 2:
            assert adjacent(1, order[i], owned) == order[j]
            assert adjacent(-1, order[i], owned) == order[j]

assert adjacent(1, order[3], {order[3]}) == order[3]
assert adjacent(-1, order[3], {order[3]}) == order[3]
assert adjacent(1, order[3], set()) == order[3]
assert adjacent(-1, order[3], set()) == order[3]
assert adjacent(1, 0, set(order)) == order[0]
assert adjacent(-1, 0, set(order)) == order[-1]
print('OK: 11 unique weapon hotkeys, consistent ordering, owned-only cycling and wrap-around.')
