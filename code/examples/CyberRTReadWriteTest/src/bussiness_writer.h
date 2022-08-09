#pragma once

#include <string>
#include <atomic>
#include "cyber/cyber.h"
#include "cyber/time/rate.h"


using apollo::cyber::Rate;
using apollo::cyber::Node;


class BussinessWriter
{
public:
    BussinessWriter(double dRate);
    ~BussinessWriter();

    void init();
    void run();

private:
    std::atomic_uint m_uiSeq;
    Rate m_objRate;
    std::unique_ptr<Node> m_upNodeTalker;

};
