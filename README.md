# GateMind: Secure Path Decision Engine

## Problem summary

Choose among **safe exit**, **trap**, or **starting point** using three boolean inputs. Rules stay explicit and complete (every combination maps somewhere), and conservative when sensors disagree.

## Inputs

| Field | Meaning |
|--------|--------|
| `alarm_active` | Facility alarm is on. |
| `guard_trusted` | Session identity checks out. |
| `door_locks_verified` | Safe-exit path reports OK from lock hardware. |

## Rules (order matters)

1. **Alarm on**  
   If locks verify the safe-exit path, send **safe exit**.  
   If not, **starting point** (do not guess a safe route during an alarm).

2. **Alarm off, trusted, locks OK**  
   **Safe exit**.

3. **Alarm off, not trusted, locks bad**  
   **Trap** (the one unambiguous bad case when the facility is quiet).

4. **Everything else**  
   **Starting point** (mixed or inconclusive signals).

Trap stays narrow: with no alarm, it is only for **untrusted session and failed locks together**—the clearest bad pairing. If only one fails, or the two disagree, route to **starting point** so noisy or conflicting telemetry does not land someone in the trap.

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

## Edge cases

- **Alarm + good locks:** Evacuation wins even if the session is untrusted.  
- **Alarm + bad locks:** No safe-exit label; **start**.  
- **Quiet + trusted + bad locks:** Identity is not enough if locks fail; **start**.  
- **Quiet + untrusted + good locks:** **Start**, not trap—trap needs both untrusted and bad locks when the alarm is off.

## Why this holds up

The core is one function with no hidden state. `main` checks all eight combinations against the table; if logic and table drift, the self-check fails. Reliability here is small surface area, an explicit table, and a loud failure when they disagree.

## How to run

```bash
g++ -std=c++17 -Wall -Wextra -pedantic src/main.cpp src/gate_mind.cpp -o gatemind
./gatemind
```

Expect a short self-check line, then three sample scenarios. Exit code `0` means the table check passed; non-zero means it did not.

## How I approached this

I fixed the truth table first, then tightened the prose. Once the eight rows held, the code structure was straightforward: alarm branch, then the quiet “happy path,” then one explicit trap case, then **start** for what is left. Explanation strings follow that order so `decide_gate` and `explain_decision` stay easy to compare.

Extra scenario tests were dropped in favor of the full table—same coverage, less noise. In a production system, the next step would be richer sensor types instead of three booleans; the branching shape would stay the same.
