#ifndef __________prodtestWebServer_EventInfo__HH
#define __________prodtestWebServer_EventInfo__HH


inline std::set<EVENT_id> getEvents_prodtestWebServer()
{

	std::set<EVENT_id> out;
	out.insert(httpEventEnum::DoListen);
	out.insert(httpEventEnum::RequestIncoming);
	out.insert(prodtestEventEnum::AddTaskREQ);
	out.insert(prodtestEventEnum::AddTaskRSP);
	out.insert(rpcEventEnum::IncomingOnAcceptor);
	out.insert(rpcEventEnum::IncomingOnConnector);
	out.insert(systemEventEnum::startService);
	out.insert(timerEventEnum::SetTimer);
	out.insert(timerEventEnum::TickTimer);

	return out;
}

inline void regEvents_prodtestWebServer()
{
	iUtils->registerEvent(httpEvent::DoListen::construct);
	iUtils->registerEvent(httpEvent::RequestIncoming::construct);
	iUtils->registerEvent(prodtestEvent::AddTaskREQ::construct);
	iUtils->registerEvent(prodtestEvent::AddTaskRSP::construct);
	iUtils->registerEvent(rpcEvent::IncomingOnAcceptor::construct);
	iUtils->registerEvent(rpcEvent::IncomingOnConnector::construct);
	iUtils->registerEvent(systemEvent::startService::construct);
	iUtils->registerEvent(timerEvent::SetTimer::construct);
	iUtils->registerEvent(timerEvent::TickTimer::construct);
}
#endif
