#ifndef __________rpc_EventInfo__HH
#define __________rpc_EventInfo__HH


inline std::set<EVENT_id> getEvents_rpc()
{

	std::set<EVENT_id> out;
	out.insert(oscarEventEnum::Accepted);
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
	out.insert(rpcEventEnum::Accepted);
	out.insert(rpcEventEnum::ConnectFailed);
	out.insert(rpcEventEnum::Connected);
	out.insert(rpcEventEnum::Disaccept);
	out.insert(rpcEventEnum::Disaccepted);
	out.insert(rpcEventEnum::Disconnect);
	out.insert(rpcEventEnum::Disconnected);
	out.insert(rpcEventEnum::IncomingOnAcceptor);
	out.insert(rpcEventEnum::IncomingOnConnector);
	out.insert(rpcEventEnum::PassPacket);
	out.insert(rpcEventEnum::SendPacket);
	out.insert(rpcEventEnum::SubscribeNotifications);
	out.insert(rpcEventEnum::UnsubscribeNotifications);
	out.insert(rpcEventEnum::UpnpPortMap);
	out.insert(rpcEventEnum::UpnpResult);
	out.insert(systemEventEnum::startService);
	out.insert(timerEventEnum::SetTimer);
	out.insert(timerEventEnum::TickAlarm);
	out.insert(timerEventEnum::TickTimer);

	return out;
}

inline void regEvents_rpc()
{
	iUtils->registerEvent(oscarEvent::Accepted::construct);
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
	iUtils->registerEvent(rpcEvent::Accepted::construct);
	iUtils->registerEvent(rpcEvent::ConnectFailed::construct);
	iUtils->registerEvent(rpcEvent::Connected::construct);
	iUtils->registerEvent(rpcEvent::Disaccept::construct);
	iUtils->registerEvent(rpcEvent::Disaccepted::construct);
	iUtils->registerEvent(rpcEvent::Disconnect::construct);
	iUtils->registerEvent(rpcEvent::Disconnected::construct);
	iUtils->registerEvent(rpcEvent::IncomingOnAcceptor::construct);
	iUtils->registerEvent(rpcEvent::IncomingOnConnector::construct);
	iUtils->registerEvent(rpcEvent::PassPacket::construct);
	iUtils->registerEvent(rpcEvent::SendPacket::construct);
	iUtils->registerEvent(rpcEvent::SubscribeNotifications::construct);
	iUtils->registerEvent(rpcEvent::UnsubscribeNotifications::construct);
	iUtils->registerEvent(rpcEvent::UpnpPortMap::construct);
	iUtils->registerEvent(rpcEvent::UpnpResult::construct);
	iUtils->registerEvent(systemEvent::startService::construct);
	iUtils->registerEvent(timerEvent::SetTimer::construct);
	iUtils->registerEvent(timerEvent::TickAlarm::construct);
	iUtils->registerEvent(timerEvent::TickTimer::construct);
}
#endif
