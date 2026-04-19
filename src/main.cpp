#include "gate_mind.hpp"
#include <cstdlib>
#include <iostream>

namespace {

void print_scenario(char const* title, GateInputs const& in) {
  std::cout << "--- " << title << " ---\n";
  std::cout << "alarm=" << (in.alarm_active ? "on" : "off")
            << "  trusted=" << (in.guard_trusted ? "yes" : "no")
            << "  locks_verified=" << (in.door_locks_verified ? "yes" : "no")
            << '\n';

  GateDecision const d = decide_gate(in);
  char const* name = nullptr;
  switch (d) {
  case GateDecision::SAFE_EXIT:
    name = "SAFE_EXIT";
    break;
  case GateDecision::TRAP:
    name = "TRAP";
    break;
  case GateDecision::START_POINT:
    name = "START_POINT";
    break;
  }

  std::cout << "decision: " << name << '\n';
  std::cout << explain_decision(in) << "\n\n";
}

bool check_case(GateInputs const& in, GateDecision expected, char const* label) {
  GateDecision const got = decide_gate(in);
  if (got == expected) {
    return true;
  }
  std::cerr << "[check failed] " << label << " (expected "
            << static_cast<int>(expected) << ", got " << static_cast<int>(got) << ")\n";
  return false;
}

// Exhaustive 3-bit input space: proves the mapping is total and stable.
bool verify_full_table() {
  struct Row {
    bool alarm;
    bool trusted;
    bool locks;
    GateDecision want;
  };
  static Row const table[] = {
      // alarm, trusted, locks
      {true, true, true, GateDecision::SAFE_EXIT},
      {true, true, false, GateDecision::START_POINT},
      {true, false, true, GateDecision::SAFE_EXIT},
      {true, false, false, GateDecision::START_POINT},
      {false, true, true, GateDecision::SAFE_EXIT},
      {false, true, false, GateDecision::START_POINT},
      {false, false, true, GateDecision::START_POINT},
      {false, false, false, GateDecision::TRAP},
  };

  bool ok = true;
  for (auto const& row : table) {
    GateInputs const in{row.alarm, row.trusted, row.locks};
    if (!check_case(in, row.want, "truth table row")) {
      ok = false;
    }
  }
  return ok;
}

bool verify_scenarios() {
  bool ok = true;

  ok &= check_case(GateInputs{false, true, true}, GateDecision::SAFE_EXIT,
                   "safe exit: quiet, trusted, locks ok");

  ok &= check_case(GateInputs{true, false, true}, GateDecision::SAFE_EXIT,
                   "safe exit: alarm overrides trust when locks verify");

  ok &= check_case(GateInputs{false, false, false}, GateDecision::TRAP,
                   "trap: quiet, untrusted, locks bad");

  ok &= check_case(GateInputs{false, true, false}, GateDecision::START_POINT,
                   "fallback: trusted but locks unverified");

  ok &= check_case(GateInputs{false, false, true}, GateDecision::START_POINT,
                   "conflicting: locks ok but not trusted");

  ok &= check_case(GateInputs{true, true, false}, GateDecision::START_POINT,
                   "alarm but path not verified");

  return ok;
}

}  // namespace

int main() {
  bool const table_ok = verify_full_table();
  bool const scenario_ok = verify_scenarios();

  if (!table_ok || !scenario_ok) {
    std::cerr << "Verification failed.\n";
    return EXIT_FAILURE;
  }
  std::cout << "All checks passed (full table + scenarios).\n\n";

  print_scenario("Routine egress", GateInputs{false, true, true});
  print_scenario("Alarm on, exit path not verified", GateInputs{true, true, false});
  print_scenario("Quiet facility, untrusted session and bad locks", GateInputs{false, false, false});

  return EXIT_SUCCESS;
}
