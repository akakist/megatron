#ifndef __________socket_EventInfo__HH
#define __________socket_EventInfo__HH


inline std::set<EVENT_id> getEvents_socket()
{

	std::set<EVENT_id> out;
	out.insert(socketEventEnum::Accepted);
	out.insert(socketEventEnum::AddToConnectTCP);
	out.insert(socketEventEnum::AddToListenTCP);
	out.insert(socketEventEnum::ConnectFailed);
	out.insert(socketEventEnum::Connected);
	out.insert(socketEventEnum::NotifyBindAddress);
	out.insert(socketEventEnum::NotifyOutBufferEmpty);
	out.insert(socketEventEnum::StreamRead);
	out.insert(socketEventEnum::Write);
	out.insert(systemEventEnum::startService);
	out.insert(timerEventEnum::TickTimer);
	out.insert(webHandlerEventEnum::RegisterHandler);
	out.insert(webHandlerEventEnum::RequestIncoming);

	return out;
}

inline void regEvents_socket()
{
	iUtils->registerEvent(socketEvent::Accepted::construct);
	iUtils->registerEvent(socketEvent::AddToConnectTCP::construct);
	iUtils->registerEvent(socketEvent::AddToListenTCP::construct);
	iUtils->registerEvent(socketEvent::ConnectFailed::construct);
	iUtils->registerEvent(socketEvent::Connected::construct);
	iUtils->registerEvent(socketEvent::NotifyBindAddress::construct);
	iUtils->registerEvent(socketEvent::NotifyOutBufferEmpty::construct);
	iUtils->registerEvent(socketEvent::StreamRead::construct);
	iUtils->registerEvent(socketEvent::Write::construct);
	iUtils->registerEvent(systemEvent::startService::construct);
	iUtils->registerEvent(timerEvent::TickTimer::construct);
	iUtils->registerEvent(webHandlerEvent::RegisterHandler::construct);
	iUtils->registerEvent(webHandlerEvent::RequestIncoming::construct);
}
#endif
