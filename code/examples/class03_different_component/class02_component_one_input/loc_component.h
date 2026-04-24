#ifndef EXAMPLES_CLASS03_DIFFERENT_COMPONENT_CLASS02_COMPONENT_ONE_INPUT_LOC_COMPONENT_H_
#define EXAMPLES_CLASS03_DIFFERENT_COMPONENT_CLASS02_COMPONENT_ONE_INPUT_LOC_COMPONENT_H_

#include <memory>
#include <string>

#include "cyber/class_loader/class_loader.h"
#include "cyber/component/component.h"
#include "cyber/component/timer_component.h"
#include "examples/class03_different_component/class02_component_one_input/proto/class02.pb.h"

namespace apollo {
namespace cyber {
namespace examples {

using apollo::cyber::TimerComponent;
using apollo::cyber::examples::proto::SimplePlanning;
using apollo::cyber::examples::proto::SimpleLocalization;
using apollo::cyber::Writer;
using apollo::cyber::Reader;

class LocComponent : public TimerComponent {
 public:
  bool Init() override;
  bool Proc() override;

 private:
  void OnPlanning(const std::shared_ptr<SimplePlanning>& msg);

  std::shared_ptr<Reader<SimplePlanning>> planning_reader_;
  std::shared_ptr<Writer<SimpleLocalization>> localization_writer_;
  uint64_t recv_count_ = 0;
  uint64_t seq_ = 0;
};
CYBER_REGISTER_COMPONENT(LocComponent)

}  // namespace examples
}  // namespace cyber
}  // namespace apollo

#endif  // EXAMPLES_CLASS03_DIFFERENT_COMPONENT_CLASS02_COMPONENT_ONE_INPUT_LOC_COMPONENT_H_
