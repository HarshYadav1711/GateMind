#include "gate_mind.hpp"

GateDecision decide_gate(GateInputs const& in) {
  // Alarm overrides normal routing: evacuate only if the safe-exit path is provably sound.
  if (in.alarm_active) {
    if (in.door_locks_verified) {
      return GateDecision::SAFE_EXIT;
    }
    return GateDecision::START_POINT;
  }

  if (in.guard_trusted && in.door_locks_verified) {
    return GateDecision::SAFE_EXIT;
  }

  // Clear hostile + tamper signal when the facility is otherwise quiet.
  if (!in.guard_trusted && !in.door_locks_verified) {
    return GateDecision::TRAP;
  }

  return GateDecision::START_POINT;
}

std::string explain_decision(GateInputs const& in) {
  GateDecision const d = decide_gate(in);

  if (d == GateDecision::SAFE_EXIT) {
    if (in.alarm_active) {
      return "Facility alarm is active and safe-exit locks are verified; evacuation takes priority.";
    }
    return "No alarm, guard session is trusted, and safe-exit locks verify; proceed to safe exit.";
  }

  if (d == GateDecision::TRAP) {
    return "No alarm, but the session is untrusted and safe-exit locks are not verified; isolate via trap.";
  }

  if (in.alarm_active && !in.door_locks_verified) {
    return "Alarm is active but safe-exit integrity is unclear; return to starting point.";
  }
  if (in.guard_trusted && !in.door_locks_verified) {
    return "Guard is trusted but locks are not verified; unclear path, return to starting point.";
  }
  if (!in.guard_trusted && in.door_locks_verified) {
    return "Locks verify but the guard session is not trusted; conflicting inputs, return to starting point.";
  }
  return "Inputs are unclear or conflicting; return to starting point.";
}
