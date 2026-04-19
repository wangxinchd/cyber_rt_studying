#ifndef EXAMPLES_CLASS01_TEST_MAINBOARD_WRITER_COMPONENT_H_
#define EXAMPLES_CLASS01_TEST_MAINBOARD_WRITER_COMPONENT_H_

#include <memory>
#include <string>

#include "cyber/class_loader/class_loader.h"
#include "cyber/component/component.h"
#include "cyber/component/timer_component.h"
#include "cyber/examples/proto/examples.pb.h"

namespace apollo {
namespace cyber {
namespace examples {

using apollo::cyber::ComponentBase;
using apollo::cyber::TimerComponent;
using apollo::cyber::examples::proto::Chatter;
using apollo::cyber::Writer;

class WriterComponent : public TimerComponent {
 public:
  bool Init() override;
  bool Proc() override;

 private:
  std::shared_ptr<Writer<Chatter>> writer_;
  uint64_t seq_ = 0;
};
CYBER_REGISTER_COMPONENT(WriterComponent)

}  // namespace examples
}  // namespace cyber
}  // namespace apollo

#endif  // EXAMPLES_CLASS01_TEST_MAINBOARD_WRITER_COMPONENT_H_
