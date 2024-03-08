#ifndef _________________JSONHANDLER_H___
#define _________________JSONHANDLER_H___

/// base class to construct JSON driven network client

#include "objectHandler.h"
#include "JAVACOOKIE_id.h"
#include "AUTHCOOKIE_id.h"
#include "IInstance.h"
#include "timerHelper.h"
#include "Events/System/timer/TickAlarm.h"
#include "Events/System/Net/jsonRef/Json.h"
#include "Events/System/Net/jsonRef/Json.h"
#include "Events/DFS/Referrer/NotifyReferrer.h"
#include "Events/DFS/Referrer/ToplinkDeliver.h"
#ifdef __ANDROID__
#include "jni.h"
#endif


#if !defined __MOBILE__

typedef void JNIEnv;
#define JBEGIN
#define JEND
#endif


enum timers
{
    T_START_CONNECT,

};

struct JsonHandler: public TimerHelper,
    public
#ifdef __MOBILE__
    ObjectHandlerThreaded
#else
    ObjectHandlerPolled
#endif
{

    JsonHandler(IInstance *ins);
    bool OH_handleObjectEvent(const REF_getter<Event::Base>& e);
    virtual bool on_JsonRSP(const jsonRefEvent::JsonRSP*)=0;

    bool on_NotifyReferrerUplinkIsConnected(const dfsReferrerEvent::NotifyReferrerUplinkIsConnected *e);
    bool on_NotifyReferrerUplinkIsDisconnected(const dfsReferrerEvent::NotifyReferrerUplinkIsDisconnected *e);
    bool on_ToplinkDeliverRSP(const dfsReferrerEvent::ToplinkDeliverRSP*);
    virtual bool handleEventInDerived(const REF_getter<Event::Base>& e)=0;

    bool on_TickAlarm(const timerEvent::TickAlarm*);




    void push_msg(const Json::Value&, const std::string &binData, const std::string &javaCookie);
    void push_err(const std::string& action,const std::string& err, const std::string &javaCookie);


#ifdef __MOBILE__
    std::pair<std::string,std::string> getMessage();
#endif

    struct __mx: public Mutexable
    {
        std::deque<std::pair<std::string,std::string> > java_msgs;
        __mx() {}

    };
    __mx mx;

    std::set<msockaddr_in> caps;

    virtual void signal_connected()=0;
    virtual void signal_disconnected()=0;
    virtual void signal_connect_failed()=0;


    bool isConnected;
    std::string config_bod;

};


#endif // JSONHANDLER_H
