#include "examples/class02_test_callback/loc_component.h"

#include "cyber/time/time.h"

namespace apollo {
namespace cyber {
namespace examples {

using apollo::cyber::Time;

bool LocComponent::Init() {
  planning_reader_ = node_->CreateReader<SimplePlanning>(
      "channel/planning",
      [this](const std::shared_ptr<SimplePlanning>& msg) {
        OnPlanning(msg);
      });
  localization_writer_ = node_->CreateWriter<SimpleLocalization>("channel/localization");
  AINFO << "LocComponent Init ok";
  return true;
}

bool LocComponent::Proc() {
  return true;
}

void LocComponent::OnPlanning(const std::shared_ptr<SimplePlanning>& msg) {
  recv_count_++;
  AINFO << "Loc received planning: seq=" << msg->seq()
        << ", x=" << msg->x() << ", y=" << msg->y()
        << ", speed=" << msg->speed()
        << ", recv_count=" << recv_count_;

  auto loc_msg = std::make_shared<SimpleLocalization>();
  loc_msg->set_timestamp(Time::Now().ToNanosecond());
  loc_msg->set_seq(msg->seq());
  loc_msg->set_x(msg->x() + 1.0);
  loc_msg->set_y(msg->y() + 1.0);
  loc_msg->set_z(0.0);
  loc_msg->set_heading(msg->heading());

  localization_writer_->Write(loc_msg);
  AINFO << "Loc sent localization: seq=" << loc_msg->seq()
        << ", x=" << loc_msg->x() << ", y=" << loc_msg->y();
}

}  // namespace examples
}  // namespace cyber
}  // namespace apollo

#include "cyber/class_loader/class_loader.h"

CYBER_REGISTER_COMPONENT(apollo::cyber::examples::LocComponent);
