#ifndef CYBER_COMPONENT_COMPONENT_WRAPPER_H_
#define CYBER_COMPONENT_COMPONENT_WRAPPER_H_

#include "cyber/component/component.h"
#include "cyber/component/component_base.h"
#include "cyber/node/node.h"
#include "cyber/node/node_wrapper.h"

namespace apollo {
namespace cyber {

template <typename M0 = NullType, typename M1 = NullType,
          typename M2 = NullType, typename M3 = NullType>
class ComponentWrapper : public Component<M0, M1, M2, M3> {
public:
  ComponentWrapper() {}
  virtual ~ComponentWrapper() {}

  bool Initialize(const ComponentConfig &config) override {
    printf("ComponentWrapper Initialize file:%s:%d\n", __FILE__, __LINE__);

    // 创建 NodeWrapper（带劫持功能）
    // 模板函数会根据 shared_ptr<NodeWrapper> 正确绑定到 NodeWrapper::CreateReader
    this->wrapper_node_.reset(new NodeWrapper(config.name()));

    if (!this->Init()) {
      AERROR << "Component Init() failed.";
      return false;
    }

    return true;
  }

protected:
  std::shared_ptr<NodeWrapper> wrapper_node_ = nullptr;

  // 返回 NodeWrapper 指针，确保劫持生效
  // 如果返回 shared_ptr<Node>，模板函数会绑定到 Node::CreateReader
  // 绕过 NodeWrapper::CreateReader 劫持逻辑
  std::shared_ptr<NodeWrapper> GetNode() {
    return wrapper_node_;
  }
};

} // namespace cyber
} // namespace apollo

#endif // CYBER_COMPONENT_COMPONENT_WRAPPER_H_
