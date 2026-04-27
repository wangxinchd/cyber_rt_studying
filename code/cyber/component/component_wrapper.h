#ifndef CYBER_COMPONENT_COMPONENT_WRAPPER_H_
#define CYBER_COMPONENT_COMPONENT_WRAPPER_H_

#include "cyber/component/component.h"
#include "cyber/component/component_base.h"
#include "cyber/node/node.h"

namespace apollo {
namespace cyber {

// ComponentWrapper - 用户继承这个类
template <typename M0 = NullType, typename M1 = NullType,
          typename M2 = NullType, typename M3 = NullType>
class ComponentWrapper : public Component<M0, M1, M2, M3> {
public:
  ComponentWrapper() {}
  virtual ~ComponentWrapper() {}

  bool Initialize(const ComponentConfig &config) override {
    fprintf(stderr, "ComponentWrapper Initialize file:%s:%d\n", __FILE__, __LINE__);
    fflush(stderr);

    this->node_.reset(new Node(config.name()));
    this->LoadConfigFiles(config);

    if (!this->Init()) {
      AERROR << "Component Init() failed.";
      return false;
    }

    return true;
  }

protected:
  std::shared_ptr<Node> GetNode() { return this->node_; }
};

} // namespace cyber
} // namespace apollo

#endif // CYBER_COMPONENT_COMPONENT_WRAPPER_H_