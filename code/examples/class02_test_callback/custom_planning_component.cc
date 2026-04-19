#include "examples/class02_test_callback/custom_planning_component.h"

#include "cyber/class_loader/class_loader.h"
#include "cyber/component/component_base.h"
#include "cyber/time/time.h"

namespace apollo {
namespace cyber {
namespace examples {

using apollo::cyber::Time;
bool CustomPlanningComponent::Init() {
  planning_component_ptr_ =
      apollo::cyber::GlobalData::Instance()
          ->GetComponentAs<apollo::cyber::TimerComponent>("PlanningComponent");

  while (apollo::cyber::OK()) {
    planning_component_ptr_->Process();
    // sleep 100ms
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  apollo::cyber::WaitForShutdown();
  return true;
}

} // namespace examples
} // namespace cyber
} // namespace apollo
