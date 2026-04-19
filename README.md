# GateMind: Secure Path Decision Engine

## Problem summary

Three doors: **safe exit**, **trap**, and **starting point**. A small piece of logic has to pick one based on facility state—without guessing when the situation is ambiguous.

## Approach

**Inputs** (all booleans):

| Input | Role |
|--------|------|
| `alarm_active` | Facility alarm (evacuation / safety context). |
| `guard_trusted` | Session or guard identity checks out. |
| `door_locks_verified` | Safe-exit path / locks report OK. |

**Priority**

1. **Alarm first.** If the alarm is on, we only send people toward **safe exit** when the safe-exit path is **verified**. If the alarm is on but locks are not verified, we do not invent a safe route—we send people back to **start**.
2. **Quiet facility.** **Safe exit** only if both **trusted** and **locks verified**.
3. **Trap** only in a narrow case: no alarm, **not** trusted, **and** locks not verified (clear tamper / unauthorized read).
4. Anything else that is mixed or weak signals defaults to **starting point**.

So: safety context can override normal trust rules for evacuation, but **never** without verified locks; ambiguity goes to **start**, except that one explicit quiet **trap** case.

## Decision table

Alarm **A**, trusted **T**, locks **L**. Outcomes: **EXIT** = safe exit, **START** = starting point, **TRAP** = trap.

| A | T | L | Outcome |
|---|---|---|---------|
| on | yes | yes | EXIT |
| on | yes | no | START |
| on | no | yes | EXIT |
| on | no | no | START |
| off | yes | yes | EXIT |
| off | yes | no | START |
| off | no | yes | START |
| off | no | no | TRAP |

## Edge cases

- **Alarm + verified locks:** Evacuation wins even if the session is untrusted—the priority is getting out when the path is known good.
- **Alarm + unverified locks:** We cannot label the path safe; **start** is the conservative choice.
- **Trusted + bad locks (no alarm):** Trusted identity does not fix bad hardware telemetry; **start**.
- **Untrusted + good locks (no alarm):** Trust and sensors disagree; **start**, not **trap** (trap needs both untrusted and bad locks when quiet).

## Why this is reliable

The function is a **total** mapping: every combination of the three booleans maps to exactly one door. There is no hidden state, no randomness, and no dependency on order of evaluation beyond the fixed rules above. The exhaustive check in `main` encodes the same table as the code—if someone changes one without the other, the program fails fast.

## How to run

From the project root (adjust paths if needed):

```bash
g++ -std=c++17 -Wall -Wextra -pedantic src/main.cpp src/gate_mind.cpp -I include -o gatemind
./gatemind
```

On success you’ll see `All checks passed`, then three sample scenarios. A non-zero exit code means a verification check failed.

## How I approached this problem

I treated it as **policy clarity**, not algorithms. The hard part is naming the inputs so each flag has one job, then ordering rules so nothing contradicts itself in an interview walkthrough. I wanted **safe exit** to require **verified locks** in every case, and I wanted a single obvious **trap** condition so it does not feel arbitrary.

The truth-table test is there because this kind of logic **will** get edited later; without a full 8-row check, it is easy to break one row and not notice. Writing the README table to match the code forces the same story in three places: implementation, tests, and explanation—which is exactly what you want when you record the video.
