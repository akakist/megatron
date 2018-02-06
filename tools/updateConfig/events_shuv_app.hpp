#ifndef __________shuv_app_EventInfo__HH
#define __________shuv_app_EventInfo__HH


inline std::set<EVENT_id> getEvents_shuv_app()
{

	std::set<EVENT_id> out;
	out.insert(dfsReferrerEventEnum::InitClient);
	out.insert(dfsReferrerEventEnum::ToplinkDeliverREQ);
	out.insert(dfsReferrerEventEnum::UpdateConfigREQ);
	out.insert(dfsReferrerEventEnum::UpdateConfigRSP);
	out.insert(jsonRefEventEnum::JsonRSP);
	out.insert(timerEventEnum::SetTimer);

	return out;
}

inline void regEvents_shuv_app()
{
	iUtils->registerEvent(dfsReferrerEvent::InitClient::construct);
	iUtils->registerEvent(dfsReferrerEvent::ToplinkDeliverREQ::construct);
	iUtils->registerEvent(dfsReferrerEvent::UpdateConfigREQ::construct);
	iUtils->registerEvent(dfsReferrerEvent::UpdateConfigRSP::construct);
	iUtils->registerEvent(jsonRefEvent::JsonRSP::construct);
	iUtils->registerEvent(timerEvent::SetTimer::construct);
}
#endif
