#ifndef __________dfsCaps_EventInfo__HH
#define __________dfsCaps_EventInfo__HH


inline std::set<EVENT_id> getEvents_dfsCaps()
{

	std::set<EVENT_id> out;
	out.insert(dfsCapsEventEnum::GetReferrersREQ);
	out.insert(dfsCapsEventEnum::GetReferrersRSP);
	out.insert(dfsCapsEventEnum::RegisterMyRefferrerREQ);
	out.insert(dfsReferrerEventEnum::ToplinkDeliverREQ);
	out.insert(dfsReferrerEventEnum::ToplinkDeliverRSP);
	out.insert(rpcEventEnum::IncomingOnAcceptor);
	out.insert(rpcEventEnum::IncomingOnConnector);
	out.insert(systemEventEnum::startService);
	out.insert(timerEventEnum::TickAlarm);
	out.insert(webHandlerEventEnum::RegisterDirectory);
	out.insert(webHandlerEventEnum::RegisterHandler);
	out.insert(webHandlerEventEnum::RequestIncoming);

	return out;
}

inline void regEvents_dfsCaps()
{
	iUtils->registerEvent(dfsCapsEvent::GetReferrersREQ::construct);
	iUtils->registerEvent(dfsCapsEvent::GetReferrersRSP::construct);
	iUtils->registerEvent(dfsCapsEvent::RegisterMyRefferrerREQ::construct);
	iUtils->registerEvent(dfsReferrerEvent::ToplinkDeliverREQ::construct);
	iUtils->registerEvent(dfsReferrerEvent::ToplinkDeliverRSP::construct);
	iUtils->registerEvent(rpcEvent::IncomingOnAcceptor::construct);
	iUtils->registerEvent(rpcEvent::IncomingOnConnector::construct);
	iUtils->registerEvent(systemEvent::startService::construct);
	iUtils->registerEvent(timerEvent::TickAlarm::construct);
	iUtils->registerEvent(webHandlerEvent::RegisterDirectory::construct);
	iUtils->registerEvent(webHandlerEvent::RegisterHandler::construct);
	iUtils->registerEvent(webHandlerEvent::RequestIncoming::construct);
}
#endif
