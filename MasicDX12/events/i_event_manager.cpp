#include "i_event_manager.h"

#include <memory>

IEventManager* g_pEventMgr = nullptr;
GenericObjectFactory<IEventData, EventTypeId> g_eventFactory;

IEventManager* IEventManager::Get() {
	return g_pEventMgr;
}

IEventDataPtr IEventManager::Create(EventTypeId eventType) {
	return IEventDataPtr(CREATE_EVENT(eventType));
}

IEventManager::IEventManager(bool setAsGlobal) {
	if (setAsGlobal) {
		if (g_pEventMgr) {
			delete g_pEventMgr;
		}
		g_pEventMgr = this;
	}
}

IEventManager::~IEventManager() {
	if (g_pEventMgr == this) {
		g_pEventMgr = nullptr;
	}
}