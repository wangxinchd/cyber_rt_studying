#include "examples/class02_test_callback/planning_component.h"

#include "cyber/time/time.h"

namespace apollo {
namespace cyber {
namespace examples {

using apollo::cyber::Time;

bool PlanningComponent::Init() {
  planning_writer_ = node_->CreateWriter<SimplePlanning>("channel/planning");
  localization_reader_ = node_->CreateReader<SimpleLocalization>(
      "channel/localization",
      [this](const std::shared_ptr<SimpleLocalization>& msg) {
        OnLocalization(msg);
      });
  AINFO << "PlanningComponent Init ok";
  return true;
}

bool PlanningComponent::Proc() {
  auto msg = std::make_shared<SimplePlanning>();
  msg->set_timestamp(Time::Now().ToNanosecond());
  msg->set_seq(seq_++);
  msg->set_x(100.0 + seq_ * 0.1);
  msg->set_y(200.0 + seq_ * 0.1);
  msg->set_heading(0.5);
  msg->set_speed(10.0);

  planning_writer_->Write(msg);
  AINFO << "Planning sent: seq=" << msg->seq()
        << ", x=" << msg->x() << ", y=" << msg->y();

  return true;
}

void PlanningComponent::OnLocalization(const std::shared_ptr<SimpleLocalization>& msg) {
  recv_count_++;
  AINFO << "Planning received localization: seq=" << msg->seq()
        << ", x=" << msg->x() << ", y=" << msg->y() << ", z=" << msg->z()
        << ", recv_count=" << recv_count_;
}

}  // namespace examples
}  // namespace cyber
}  // namespace apollo

#include "cyber/class_loader/class_loader.h"

CYBER_REGISTER_COMPONENT(apollo::cyber::examples::PlanningComponent);
