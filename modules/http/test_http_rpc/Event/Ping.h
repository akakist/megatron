#ifndef _________dfsReferrerEvent_h19Z1
#define _________dfsReferrerEvent_h19Z1
#include "___prodtestEvent.h"
#include <vector>


namespace prodtestEvent {




    class AddTaskREQ: public Event::Base
    {


    public:
        static Base* construct(const route_t &r)
        {
            return new AddTaskREQ(r);
        }
        AddTaskREQ(const std::string& _session,
                   const std::string& _sampleString,int _count,
                   const route_t&r)
            :Base(prodtestEventEnum::AddTaskREQ,r),
             session(_session),
             sampleString(_sampleString),count(_count)
        {}
        AddTaskREQ(const route_t&r)
            :Base(prodtestEventEnum::AddTaskREQ,r) {}
        std::string session;
        std::string sampleString;
        int count;
        void unpack(inBuffer& o)
        {
            o>>session>>sampleString>>count;
        }
        void pack(outBuffer&o) const
        {
            o<<session<<sampleString<<count;
        }
        void jdump(Json::Value &) const
        {
        }

    };

    class AddTaskRSP: public Event::Base
    {


    public:
        static Base* construct(const route_t &r)
        {
            return new AddTaskRSP(r);
        }
        AddTaskRSP(const std::string& _session,
                   const std::string& _sampleString,int _count,
                   const route_t&r)
            :Base(prodtestEventEnum::AddTaskRSP,r),
             session(_session),
             sampleString(_sampleString),count(_count)
        {}
        AddTaskRSP(const route_t&r)
            :Base(prodtestEventEnum::AddTaskRSP,r) {}
        std::string session;
        std::string sampleString;
        int count;
        void unpack(inBuffer& o)
        {
            o>>session>>sampleString>>count;
        }
        void pack(outBuffer&o) const
        {
            o<<session<<sampleString<<count;
        }
        void jdump(Json::Value &) const
        {
        }

    };







}
#endif
