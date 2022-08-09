#pragma once

#include <string>
#include "cyber/cyber.h"
#include "bussiness_data_info.pb.h"


using apollo::cyber::Node;
using apollo::cyber::examples::proto::BussinessDataInfo;

class BussinessReader
{
public:
    BussinessReader();
    ~BussinessReader();

    void init();
    void run();

private:
    void handleMsg(const std::shared_ptr<BussinessDataInfo>& spDataInfo);

private:
    std::unique_ptr<Node> m_upNodeListener;

};
