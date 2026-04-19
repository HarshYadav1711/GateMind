#include "../include/gate_mind.hpp"
#include <cstdlib>
#include <iostream>
#include <iterator>

namespace {

char const* decision_name(GateDecision d) {
  switch (d) {
  case GateDecision::SAFE_EXIT:
    return "SAFE_EXIT";
  case GateDecision::TRAP:
    return "TRAP";
  case GateDecision::START_POINT:
    return "START_POINT";
  default:
    return "?";
  }
}

void print_scenario(char const* title, GateInputs const& in) {
  std::cout << title << '\n';
  std::cout << "  alarm=" << (in.alarm_active ? "on" : "off")
            << "  trusted=" << (in.guard_trusted ? "yes" : "no")
            << "  locks_verified=" << (in.door_locks_verified ? "yes" : "no") << '\n';

  GateDecision const d = decide_gate(in);
  std::cout << "  -> " << decision_name(d) << '\n';
  std::cout << "  " << explain_decision(in) << "\n\n";
}

bool expect(GateInputs const& in, GateDecision want, std::size_t row_index) {
  GateDecision const got = decide_gate(in);
  if (got == want) {
    return true;
  }
  std::cerr << "FAIL row " << row_index << " (alarm=" << (in.alarm_active ? "on" : "off")
            << " trusted=" << (in.guard_trusted ? "yes" : "no")
            << " locks_verified=" << (in.door_locks_verified ? "yes" : "no") << ")"
            << " - expected " << decision_name(want) << ", got " << decision_name(got) << '\n';
  return false;
}

bool run_truth_table() {
  struct Row {
    bool alarm;
    bool trusted;
    bool locks;
    GateDecision want;
  };
  static Row const kTable[] = {
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
  for (std::size_t i = 0; i < std::size(kTable); ++i) {
    Row const& r = kTable[i];
    GateInputs const in{r.alarm, r.trusted, r.locks};
    if (!expect(in, r.want, i)) {
      ok = false;
    }
  }
  return ok;
}

}  // namespace

int main() {
  if (!run_truth_table()) {
    std::cerr << "Self-check failed.\n";
    return EXIT_FAILURE;
  }

  std::cout << "Self-check passed: all 8 combinations match the decision table.\n\n";

  print_scenario("1. Routine egress", GateInputs{false, true, true});
  print_scenario("2. Alarm on, safe-exit path not verified", GateInputs{true, true, false});
  print_scenario("3. Quiet site, untrusted session and bad locks", GateInputs{false, false, false});

  return EXIT_SUCCESS;
}
