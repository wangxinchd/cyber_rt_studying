#ifndef EXAMPLES_CLASS03_DIFFERENT_COMPONENT_CLASS02_COMPONENT_ONE_INPUT_PLANNING_COMPONENT_H_
#define EXAMPLES_CLASS03_DIFFERENT_COMPONENT_CLASS02_COMPONENT_ONE_INPUT_PLANNING_COMPONENT_H_

#include <memory>
#include <string>

#include "cyber/class_loader/class_loader.h"
#include "cyber/component/component.h"
#include "examples/class03_different_component/class02_component_one_input/proto/class02.pb.h"

namespace apollo {
namespace cyber {
namespace examples {

using apollo::cyber::Component;
using apollo::cyber::examples::proto::SimplePlanning;
using apollo::cyber::examples::proto::SimpleLocalization;
using apollo::cyber::Writer;

class PlanningComponent : public Component<SimpleLocalization> {
 public:
  bool Init() override;
  bool Proc(const std::shared_ptr<SimpleLocalization>& msg) override;

 private:
  std::shared_ptr<Writer<SimplePlanning>> planning_writer_;
  uint64_t seq_ = 0;
  uint64_t recv_count_ = 0;
};
CYBER_REGISTER_COMPONENT(PlanningComponent)

}  // namespace examples
}  // namespace cyber
}  // namespace apollo

#endif  // EXAMPLES_CLASS03_DIFFERENT_COMPONENT_CLASS02_COMPONENT_ONE_INPUT_PLANNING_COMPONENT_H_
