#include "examples/class03_different_component/class02_component_one_input/loc_component.h"

#include "cyber/time/time.h"

namespace apollo {
namespace cyber {
namespace examples {

using apollo::cyber::Time;

bool LocComponent::Init() {
  planning_reader_ = node_->CreateReader<SimplePlanning>(
      "channel/planning_traj",
      [this](const std::shared_ptr<SimplePlanning>& msg) {
        OnPlanning(msg);
      });
  localization_writer_ = node_->CreateWriter<SimpleLocalization>("channel/localization");
  AINFO << "LocComponent Init ok";
  return true;
}

bool LocComponent::Proc() {
  auto loc_msg = std::make_shared<SimpleLocalization>();
  loc_msg->set_timestamp(Time::Now().ToNanosecond());
  loc_msg->set_seq(++seq_);
  loc_msg->set_x(100.0 + seq_ * 0.1);
  loc_msg->set_y(200.0 + seq_ * 0.1);
  loc_msg->set_z(0.0);
  loc_msg->set_heading(0.5);

  localization_writer_->Write(loc_msg);
  AINFO << "Loc sent localization: seq=" << loc_msg->seq()
        << ", x=" << loc_msg->x() << ", y=" << loc_msg->y();
  return true;
}

void LocComponent::OnPlanning(const std::shared_ptr<SimplePlanning>& msg) {
  recv_count_++;
  AINFO << "Loc received planning_traj: seq=" << msg->seq()
        << ", x=" << msg->x() << ", y=" << msg->y()
        << ", speed=" << msg->speed()
        << ", recv_count=" << recv_count_;
}

}  // namespace examples
}  // namespace cyber
}  // namespace apollo
