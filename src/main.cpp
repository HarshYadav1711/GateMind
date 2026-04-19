#include "gate_mind.hpp"
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

}  // namespace

int main() {
  print_scenario("Routine egress", GateInputs{false, true, true});

  print_scenario("Alarm on, exit path not verified", GateInputs{true, true, false});

  print_scenario("Quiet facility, untrusted session and bad locks", GateInputs{false, false, false});
}
