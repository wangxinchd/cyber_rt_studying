#ifndef CYBER_NODE_NODE_WRAPPER_H_
#define CYBER_NODE_NODE_WRAPPER_H_

#include <memory>
#include <string>

#include "cyber/cyber.h"
#include "cyber/node/node.h"

namespace apollo {
namespace cyber {

/**
 * @class NodeWrapper
 * @brief 劫持 Node 的关键方法，用于调试和监控
 *
 * 继承 Node，重写关键方法实现劫持/监控
 */
class NodeWrapper : public Node {
public:
  explicit NodeWrapper(const std::string &node_name,
                       const std::string &name_space = "")
      : Node(node_name, name_space) {
    fprintf(stderr, "NodeWrapper created for: %s\n", node_name.c_str());
    fflush(stderr);
  }

  ~NodeWrapper() {
    fprintf(stderr, "NodeWrapper destroyed\n");
    fflush(stderr);
  }

  template <typename MessageT>
  auto CreateWriter(const std::string &channel_name)
      -> std::shared_ptr<Writer<MessageT>> {
    fprintf(stderr, "NodeWrapper::CreateWriter: %s\n", channel_name.c_str());
    fflush(stderr);
    return Node::CreateWriter<MessageT>(channel_name);
  }

  template <typename MessageT>
  auto CreateReader(const std::string &channel_name,
                    const CallbackFunc<MessageT> &reader_func = nullptr)
      -> std::shared_ptr<cyber::Reader<MessageT>> {
    fprintf(stderr, "NodeWrapper::CreateReader: %s\n", channel_name.c_str());
    fflush(stderr);
    return Node::CreateReader<MessageT>(channel_name, reader_func);
  }
};

} // namespace cyber
} // namespace apollo

#endif // CYBER_NODE_NODE_WRAPPER_H_
