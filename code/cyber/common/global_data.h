/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef CYBER_COMMON_GLOBAL_DATA_H_
#define CYBER_COMMON_GLOBAL_DATA_H_

#include <string>
#include <unordered_map>

#include "cyber/proto/cyber_conf.pb.h"

#include "cyber/base/atomic_hash_map.h"
#include "cyber/base/atomic_rw_lock.h"
#include "cyber/common/log.h"
#include "cyber/common/macros.h"
#include "cyber/common/util.h"

namespace apollo {
namespace cyber {
namespace common {

using ::apollo::cyber::base::AtomicHashMap;
using ::apollo::cyber::proto::ClockMode;
using ::apollo::cyber::proto::CyberConfig;
using ::apollo::cyber::proto::RunMode;
using ::apollo::cyber::base::AtomicRWLock;
using ::apollo::cyber::base::ReadLockGuard;
using ::apollo::cyber::base::WriteLockGuard;

class GlobalData {
 public:
  ~GlobalData();

  int ProcessId() const;

  void SetProcessGroup(const std::string& process_group);
  const std::string& ProcessGroup() const;

  void SetComponentNums(const int component_nums);
  int ComponentNums() const;

  void SetSchedName(const std::string& sched_name);
  const std::string& SchedName() const;

  const std::string& HostIp() const;

  const std::string& HostName() const;

  const CyberConfig& Config() const;

  void EnableSimulationMode();
  void DisableSimulationMode();

  bool IsRealityMode() const;
  bool IsMockTimeMode() const;

  static uint64_t GenerateHashId(const std::string& name) {
    return common::Hash(name);
  }

  static uint64_t RegisterNode(const std::string& node_name);
  static std::string GetNodeById(uint64_t id);

  static uint64_t RegisterChannel(const std::string& channel);
  static std::string GetChannelById(uint64_t id);

  static uint64_t RegisterService(const std::string& service);
  static std::string GetServiceById(uint64_t id);

  static uint64_t RegisterTaskName(const std::string& task_name);
  static std::string GetTaskNameById(uint64_t id);

  bool RegisterComponent(const std::string& node_name, const std::shared_ptr<void>& component_ptr);
  template<typename T>
  std::shared_ptr<T> GetComponentAs(const std::string& node_name) {
    if (node_name.empty()) {
      return nullptr;
    }
    ReadLockGuard<AtomicRWLock> lock(component_map_lock_);
    auto it = node_component_map_.find(node_name);
    if (it != node_component_map_.end()) {
      return std::static_pointer_cast<T>(it->second);
    }
    else {
      AERROR<< "no component of " << node_name;
      return nullptr;
    }
  }

  void ClearComponent() {
    ReadLockGuard<AtomicRWLock> lock(component_map_lock_);
    node_component_map_.clear();
  }

 private:
  void InitHostInfo();
  bool InitConfig();

  // global config
  CyberConfig config_;

  // host info
  std::string host_ip_;
  std::string host_name_;

  // process info
  int process_id_;
  std::string process_group_;

  int component_nums_ = 0;

  // sched policy info
  std::string sched_name_ = "CYBER_DEFAULT";

  // run mode
  RunMode run_mode_;
  ClockMode clock_mode_;
  AtomicRWLock component_map_lock_{true};

  static AtomicHashMap<uint64_t, std::string, 512> node_id_map_;
  static AtomicHashMap<uint64_t, std::string, 256> channel_id_map_;
  static AtomicHashMap<uint64_t, std::string, 256> service_id_map_;
  static AtomicHashMap<uint64_t, std::string, 256> task_id_map_;
  std::unordered_map<std::string, std::shared_ptr<void>> node_component_map_;

  DECLARE_SINGLETON(GlobalData)
};

}  // namespace common
}  // namespace cyber
}  // namespace apollo

#endif  // CYBER_COMMON_GLOBAL_DATA_H_
