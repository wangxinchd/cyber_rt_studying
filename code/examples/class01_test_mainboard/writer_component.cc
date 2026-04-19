#include "examples/class01_test_mainboard/writer_component.h"

#include "cyber/time/time.h"

namespace apollo {
namespace cyber {
namespace examples {

using apollo::cyber::Time;

bool WriterComponent::Init() {
  writer_ = node_->CreateWriter<Chatter>("channel/class01_test");
  AINFO << "WriterComponent Init ok";
  return true;
}

bool WriterComponent::Proc() {
  auto msg = std::make_shared<Chatter>();
  msg->set_timestamp(Time::Now().ToNanosecond());
  msg->set_seq(seq_++);
  msg->set_content("Hello from planning!");

  writer_->Write(msg);
  AINFO << "Writer sent seq=" << msg->seq();

  return true;
}

}  // namespace examples
}  // namespace cyber
}  // namespace apollo
