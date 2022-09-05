#include "evt_data_request_start_game.h"

const std::string EvtData_Request_Start_Game::sk_EventName = "EvtData_Request_Start_Game";

EvtData_Request_Start_Game::EvtData_Request_Start_Game() {}

EventTypeId EvtData_Request_Start_Game::VGetEventType() const {
	return sk_EventType;
}

void EvtData_Request_Start_Game::VSerialize(std::ostream& out) const {}

void EvtData_Request_Start_Game::VDeserialize(std::istream& in) {}

IEventDataPtr EvtData_Request_Start_Game::VCopy() const {
	return IEventDataPtr(new EvtData_Request_Start_Game());
}

const std::string& EvtData_Request_Start_Game::GetName() const {
	return sk_EventName;
}
