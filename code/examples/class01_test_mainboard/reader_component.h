#ifndef EXAMPLES_CLASS01_TEST_MAINBOARD_READER_COMPONENT_H_
#define EXAMPLES_CLASS01_TEST_MAINBOARD_READER_COMPONENT_H_

#include <memory>
#include <string>

#include "cyber/class_loader/class_loader.h"
#include "cyber/component/component.h"
#include "cyber/examples/proto/examples.pb.h"

namespace apollo {
namespace cyber {
namespace examples {

using apollo::cyber::Component;
using apollo::cyber::ComponentBase;
using apollo::cyber::examples::proto::Chatter;
using apollo::cyber::Reader;

class ReaderComponent : public Component<Chatter> {
 public:
  bool Init() override;
  bool Proc(const std::shared_ptr<Chatter>& msg) override;

 private:
  std::shared_ptr<Reader<Chatter>> reader_;
  uint64_t recv_count_ = 0;
};

}  // namespace examples
}  // namespace cyber
}  // namespace apollo

#endif  // EXAMPLES_CLASS01_TEST_MAINBOARD_READER_COMPONENT_H_