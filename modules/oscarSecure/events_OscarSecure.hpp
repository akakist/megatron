#ifndef __________OscarSecure_EventInfo__HH
#define __________OscarSecure_EventInfo__HH


inline std::set<EVENT_id> getEvents_OscarSecure()
{

	std::set<EVENT_id> out;
	out.insert(errorDispatcherEventEnum::SendMessage);
	out.insert(oscarEventEnum::AddToListenTCP);
	out.insert(oscarEventEnum::Connect);
	out.insert(oscarEventEnum::ConnectFailed);
	out.insert(oscarEventEnum::Connected);
	out.insert(oscarEventEnum::NotifyBindAddress);
	out.insert(oscarEventEnum::NotifyOutBufferEmpty);
	out.insert(oscarEventEnum::PacketOnAcceptor);
	out.insert(oscarEventEnum::PacketOnConnector);
	out.insert(oscarEventEnum::SendPacket);
	out.insert(oscarEventEnum::SocketClosed);
	out.insert(socketEventEnum::Accepted);
	out.insert(socketEventEnum::AddToConnectTCP);
	out.insert(socketEventEnum::AddToListenTCP);
	out.insert(socketEventEnum::ConnectFailed);
	out.insert(socketEventEnum::Connected);
	out.insert(socketEventEnum::NotifyBindAddress);
	out.insert(socketEventEnum::NotifyOutBufferEmpty);
	out.insert(socketEventEnum::StreamRead);
	out.insert(systemEventEnum::startService);

	return out;
}

inline void regEvents_OscarSecure()
{
	iUtils->registerEvent(errorDispatcherEvent::SendMessage::construct);
	iUtils->registerEvent(oscarEvent::AddToListenTCP::construct);
	iUtils->registerEvent(oscarEvent::Connect::construct);
	iUtils->registerEvent(oscarEvent::ConnectFailed::construct);
	iUtils->registerEvent(oscarEvent::Connected::construct);
	iUtils->registerEvent(oscarEvent::NotifyBindAddress::construct);
	iUtils->registerEvent(oscarEvent::NotifyOutBufferEmpty::construct);
	iUtils->registerEvent(oscarEvent::PacketOnAcceptor::construct);
	iUtils->registerEvent(oscarEvent::PacketOnConnector::construct);
	iUtils->registerEvent(oscarEvent::SendPacket::construct);
	iUtils->registerEvent(oscarEvent::SocketClosed::construct);
	iUtils->registerEvent(socketEvent::Accepted::construct);
	iUtils->registerEvent(socketEvent::AddToConnectTCP::construct);
	iUtils->registerEvent(socketEvent::AddToListenTCP::construct);
	iUtils->registerEvent(socketEvent::ConnectFailed::construct);
	iUtils->registerEvent(socketEvent::Connected::construct);
	iUtils->registerEvent(socketEvent::NotifyBindAddress::construct);
	iUtils->registerEvent(socketEvent::NotifyOutBufferEmpty::construct);
	iUtils->registerEvent(socketEvent::StreamRead::construct);
	iUtils->registerEvent(systemEvent::startService::construct);
}
#endif
