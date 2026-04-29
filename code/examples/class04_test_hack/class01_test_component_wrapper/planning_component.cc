#include "planning_component.h"

#include "cyber/time/time.h"

namespace apollo {
namespace cyber {
namespace examples {

using apollo::cyber::Time;

bool PlanningComponent::Init() {
  AINFO << "PlanningComponent Init ok";

  planning_writer_ =
      this->GetNode()->CreateWriter<SimplePlanning>("channel/planning_traj");

  localization_reader_ = this->GetNode()->CreateReader<SimpleLocalization>(
      "channel/localization",
      [this](const std::shared_ptr<SimpleLocalization> &msg) {
        OnLocalization(msg);
      });

  return true;
}

void PlanningComponent::OnLocalization(
    const std::shared_ptr<SimpleLocalization> &msg) {
  recv_count_++;
  AINFO << "Planning received localization: seq=" << msg->seq()
        << ", x=" << msg->x() << ", y=" << msg->y() << ", z=" << msg->z()
        << ", timestamp=" << msg->timestamp() << ", recv_count=" << recv_count_;

  auto plan_msg = std::make_shared<SimplePlanning>();
  plan_msg->set_timestamp(Time::Now().ToNanosecond());
  plan_msg->set_seq(msg->seq());
  plan_msg->set_x(msg->x() + 10.0);
  plan_msg->set_y(msg->y() + 10.0);
  plan_msg->set_heading(msg->heading());
  plan_msg->set_speed(5.0);
  plan_msg->set_localization_timestamp(msg->timestamp());
  plan_msg->set_localization_seq(msg->seq());

  // 模拟计算规划轨迹时间 这个时间写成一个随机数 100-199 毫秒
  std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100 + 100));

  planning_writer_->Write(plan_msg);
  AINFO << "Planning sent planning_traj: seq=" << plan_msg->seq()
        << ", x=" << plan_msg->x() << ", y=" << plan_msg->y()
        << ", localization_timestamp=" << plan_msg->localization_timestamp()
        << ", localization_seq=" << plan_msg->localization_seq();
}

} // namespace examples
} // namespace cyber
} // namespace apollo
