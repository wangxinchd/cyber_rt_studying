#ifndef EXAMPLES_CLASS02_TEST_CALLBACK_PLANNING_COMPONENT_H_
#define EXAMPLES_CLASS02_TEST_CALLBACK_PLANNING_COMPONENT_H_

#include <memory>
#include <string>

#include "cyber/class_loader/class_loader.h"
#include "cyber/component/component.h"
#include "cyber/component/timer_component.h"
#include "examples/class02_test_callback/proto/class02.pb.h"

namespace apollo {
namespace cyber {
namespace examples {

using apollo::cyber::ComponentBase;
using apollo::cyber::TimerComponent;
using apollo::cyber::examples::proto::SimplePlanning;
using apollo::cyber::examples::proto::SimpleLocalization;
using apollo::cyber::Writer;
using apollo::cyber::Reader;

class PlanningComponent : public TimerComponent {
 public:
  bool Init() override;
  bool Proc() override;

 private:
  void OnLocalization(const std::shared_ptr<SimpleLocalization>& msg);

  std::shared_ptr<Writer<SimplePlanning>> planning_writer_;
  std::shared_ptr<Reader<SimpleLocalization>> localization_reader_;
  uint64_t seq_ = 0;
  uint64_t recv_count_ = 0;
};

}  // namespace examples
}  // namespace cyber
}  // namespace apollo

#endif  // EXAMPLES_CLASS02_TEST_CALLBACK_PLANNING_COMPONENT_H_
