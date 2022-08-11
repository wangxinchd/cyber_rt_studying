#include "bussiness_writer.h"
#include "bussiness_global.h"
#include "cyber/time/time.h"
#include "bussiness_data_info.pb.h"


using apollo::cyber::Time;
using apollo::cyber::Writer;
using apollo::cyber::examples::proto::BussinessDataInfo;


BussinessWriter::BussinessWriter(double dRate)
    : m_uiSeq(0)
    , m_objRate(dRate)
{

}

BussinessWriter::~BussinessWriter()
{

}

void BussinessWriter::init()
{
    // create talker node
    m_upNodeTalker = apollo::cyber::CreateNode(TALKER);

    return;
}

void BussinessWriter::run()
{
    // create talker
    std::shared_ptr<Writer<BussinessDataInfo> > talker = m_upNodeTalker->CreateWriter<BussinessDataInfo>(CHANNEL);
    talker->Init();
    std::cout << "cyber rt state " << apollo::cyber::OK() << std::endl;
    while (apollo::cyber::OK())
    {
        auto msg = std::make_shared<BussinessDataInfo>();
        msg->set_timestamp(Time::Now().ToNanosecond());
        msg->set_seq(m_uiSeq++);
        msg->set_tag(156);
        msg->set_msg("Hello, apollo, this is test message!");
        bool bRet = talker->Write(msg);
        std::cout << "writer sent a message, ret: " << bRet << ", timestamp: " << msg->timestamp() << ", seq: " << msg->seq() << std::endl;
        //AINFO << "writer sent a message!";
        m_objRate.Sleep();
    }

    return;
}

