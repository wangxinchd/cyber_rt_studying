#pragma once

#include <memory>
#include <string>

#include "cyber/class_loader/class_loader.h"
#include "cyber/component/component_wrapper.h"
#include "proto/class02.pb.h"

namespace apollo {
namespace cyber {
namespace examples {

using apollo::cyber::ComponentWrapper;
using apollo::cyber::examples::proto::SimplePlanning;
using apollo::cyber::examples::proto::SimpleLocalization;
using apollo::cyber::Writer;

class PlanningComponent : public ComponentWrapper<> {
 public:
  bool Init() override;

 private:
  // std::shared_ptr<Writer<SimplePlanning>> planning_writer_;
  uint64_t seq_ = 0;
  uint64_t recv_count_ = 0;
};
CYBER_REGISTER_COMPONENT(PlanningComponent)

}  // namespace examples
}  // namespace cyber
}  // namespace apollo

