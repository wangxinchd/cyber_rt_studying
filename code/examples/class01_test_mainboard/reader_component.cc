#include "examples/class01_test_mainboard/reader_component.h"

namespace apollo {
namespace cyber {
namespace examples {

using apollo::cyber::examples::proto::Chatter;

bool ReaderComponent::Init() {
  return true;
}

bool ReaderComponent::Proc(const std::shared_ptr<Chatter>& msg) {
  recv_count_++;
  AINFO << "Reader received: seq=" << msg->seq()
        << ", content=" << msg->content()
        << ", recv_count=" << recv_count_;
  return true;
}

}  // namespace examples
}  // namespace cyber
}  // namespace apollo

#include "cyber/class_loader/class_loader.h"

CYBER_REGISTER_COMPONENT(apollo::cyber::examples::ReaderComponent);