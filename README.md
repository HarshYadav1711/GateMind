# GateMind: Secure Path Decision Engine

## Problem summary

Pick one of three doors: **safe exit**, **trap**, or **starting point**, from three boolean inputs. The rules need to be obvious, total (every input combination maps somewhere), and conservative when sensors disagree.

## Inputs

| Field | Meaning |
|--------|--------|
| `alarm_active` | Facility alarm is on. |
| `guard_trusted` | Identity / session checks out. |
| `door_locks_verified` | Safe-exit path reports OK from lock hardware. |

## Rules (order matters)

1. **Alarm on**  
   If locks verify the safe-exit path, send **safe exit**.  
   If not, **starting point** (do not guess a safe route during an alarm).

2. **Alarm off, trusted, locks OK**  
   **Safe exit**.

3. **Alarm off, not trusted, locks bad**  
   **Trap** (single clear “bad” case when the building is otherwise quiet).

4. **Everything else**  
   **Starting point** (mixed or weak signals).

Safe exit always requires **verified locks**. Trust can be ignored for evacuation only when the alarm is on **and** locks verify.

## Decision table

`A` = alarm, `T` = trusted, `L` = locks verified.

| A | T | L | Outcome |
|---|---|---|---------|
| on | yes | yes | Safe exit |
| on | yes | no | Start |
| on | no | yes | Safe exit |
| on | no | no | Start |
| off | yes | yes | Safe exit |
| off | yes | no | Start |
| off | no | yes | Start |
| off | no | no | Trap |

## Edge cases worth stating

- **Alarm + good locks:** Evacuation wins even if the session is untrusted.  
- **Alarm + bad locks:** No safe-exit label; **start**.  
- **Quiet + trusted + bad locks:** Identity is not enough if locks fail; **start**.  
- **Quiet + untrusted + good locks:** **Start**, not trap—trap needs both untrusted and bad locks when the alarm is off.

## Why this holds up

The core is one function with no hidden state. The program checks all eight input combinations against the table in `main`; if the logic and the table drift, the self-check fails. That is the whole reliability story: small surface area, explicit table, fail loud.

## How to run

```bash
g++ -std=c++17 -Wall -Wextra -pedantic src/main.cpp src/gate_mind.cpp -I include -o gatemind
./gatemind
```

You should see a one-line self-check, then three sample scenarios. Exit code `0` means the table check passed; non-zero means it did not.

## How I approached this

I wrote the truth table before polishing prose. Once the eight rows were stable, the code almost wrote itself: alarm branch first, then the quiet “happy path,” then one explicit trap case, then **start** for the remaining rows. The explanation strings follow that same order so a reviewer can read `decide_gate` and `explain_decision` side by side without hunting for mismatches.

The redundant “scenario” tests were dropped in favor of the full table only—same coverage, less noise. If this were production code, the next step would be feeding real sensor enums instead of three booleans, but the structure would stay the same.
