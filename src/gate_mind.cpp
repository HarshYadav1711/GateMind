#include "gate_mind.hpp"

GateDecision decide_gate(GateInputs const& in) {
  if (in.alarm_active) {
    if (in.door_locks_verified) {
      return GateDecision::SAFE_EXIT;
    }
    return GateDecision::START_POINT;
  }

  if (in.guard_trusted && in.door_locks_verified) {
    return GateDecision::SAFE_EXIT;
  }

  if (!in.guard_trusted && !in.door_locks_verified) {
    return GateDecision::TRAP;
  }

  return GateDecision::START_POINT;
}

std::string explain_decision(GateInputs const& in) {
  // Same order as decide_gate so the story matches the branch structure.
  if (in.alarm_active) {
    if (in.door_locks_verified) {
      return "Alarm is on and the safe-exit path is verified: use the safe exit.";
    }
    return "Alarm is on but the safe-exit path is not verified: return to the starting point.";
  }

  if (in.guard_trusted && in.door_locks_verified) {
    return "No alarm, session is trusted, locks verify: safe exit.";
  }

  if (!in.guard_trusted && !in.door_locks_verified) {
    return "No alarm, session not trusted, locks not verified: route to the trap.";
  }

  if (in.guard_trusted && !in.door_locks_verified) {
    return "Session is trusted but locks are not verified: return to the starting point.";
  }

  return "Locks verify but session is not trusted: conflicting inputs, return to the starting point.";
}
