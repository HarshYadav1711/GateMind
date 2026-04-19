#pragma once

#include <string>

enum class GateDecision {
  SAFE_EXIT,
  TRAP,
  START_POINT
};

struct GateInputs {
  bool alarm_active{};
  bool guard_trusted{};
  bool door_locks_verified{};
};

GateDecision decide_gate(GateInputs const& in);

std::string explain_decision(GateInputs const& in);
