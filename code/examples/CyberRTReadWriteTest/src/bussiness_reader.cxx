#include "bussiness_reader.h"
#include "bussiness_global.h"
#include "cyber/time/time.h"

using apollo::cyber::Time;

static void pfnMessageCallback(const std::shared_ptr<BussinessDataInfo>& spDataInfo)
{
    std::cout << "Listener [" << Time::Now().ToNanosecond() << "] recv info timestamp ("
              << spDataInfo->timestamp() << ") seq (" << spDataInfo->seq()
              << ") tag (" << spDataInfo->tag() << "), message: "
              << spDataInfo->msg() << std::endl;

    return;
}



BussinessReader::BussinessReader()
{

}

BussinessReader::~BussinessReader()
{

}

void BussinessReader::init()
{
    // create listener node
    m_upNodeListener = apollo::cyber::CreateNode(LISTENER);

    return;
}

void BussinessReader::run()
{
    // create listener
    // method 1
    //auto listener = m_upNodeListener->CreateReader<apollo::cyber::examples::proto::BussinessDataInfo>(CHANNEL, pfnMessageCallback);
    // method 2
    auto listener = m_upNodeListener->CreateReader<BussinessDataInfo>(CHANNEL
                                                                      , [this](const std::shared_ptr<BussinessDataInfo>& spDataInfo) {
        this->handleMsg(spDataInfo);
    });

    apollo::cyber::WaitForShutdown();

    return;
}

void BussinessReader::handleMsg(const std::shared_ptr<BussinessDataInfo>& spDataInfo)
{
    std::cout << "Listener [" << Time::Now().ToNanosecond() << "] recv info timestamp ("
              << spDataInfo->timestamp() << ") seq (" << spDataInfo->seq()
              << ") tag (" << spDataInfo->tag() << "), message: "
              << spDataInfo->msg() << std::endl;
    // TODO

    return;
}


