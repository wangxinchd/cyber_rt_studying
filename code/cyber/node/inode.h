#pragma once

#include <memory>
#include <string>
#include <mutex>
#include <map>

#include "cyber/cyber.h"
#include "cyber/node/node_channel_impl.h"
#include "cyber/node/node_service_impl.h"
#include "cyber/reader.h"
#include "cyber/writer.h"

namespace apollo {
namespace cyber {

/**
 * @class INode
 * @brief 节点接口基类（仅作为标记接口）
 */
class INode {
public:
  virtual ~INode() {}
};

} // namespace cyber
} // namespace apollo
