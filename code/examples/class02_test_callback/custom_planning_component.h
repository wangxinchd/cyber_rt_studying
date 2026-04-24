#ifndef EXAMPLES_CLASS02_TEST_CALLBACK_CUSTOM_PLANNING_COMPONENT_H_
#define EXAMPLES_CLASS02_TEST_CALLBACK_CUSTOM_PLANNING_COMPONENT_H_

#include <memory>
#include <string>

#include "cyber/class_loader/class_loader.h"
#include "cyber/component/component.h"
#include "cyber/component/timer_component.h"
#include "examples/class02_test_callback/proto/class02.pb.h"

namespace apollo {
namespace cyber {
namespace examples {

using apollo::cyber::Reader;
using apollo::cyber::Component;
using apollo::cyber::Writer;
using apollo::cyber::examples::proto::SimpleLocalization;
using apollo::cyber::examples::proto::SimplePlanning;

class CustomPlanningComponent : public Component<> {
 public:
  bool Init() override;

 private:
  std::shared_ptr<TimerComponent> planning_component_ptr_ = nullptr;
};

CYBER_REGISTER_COMPONENT(CustomPlanningComponent)

}  // namespace examples
}  // namespace cyber
}  // namespace apollo

#endif  // EXAMPLES_CLASS02_TEST_CALLBACK_CUSTOM_PLANNING_COMPONENT_H_
