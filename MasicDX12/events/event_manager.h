#pragma once

#include <iostream>
#include <unordered_map>
#include <list>
#include <string>

#include "i_event_manager.h"

const unsigned int EVENTMANAGER_NUM_QUEUES = 2;

class EventManager : public IEventManager {
	std::unordered_map<EventTypeId, std::list<EventListenerDelegate>> m_eventListeners;
	const std::string m_eventManagerName;

	std::list<IEventDataPtr> m_queues[EVENTMANAGER_NUM_QUEUES];
	int m_activeQueue;

public:
	explicit EventManager(const std::string& pName, bool setAsGlobal);

	bool VAddListener(const EventListenerDelegate& eventDelegate, const EventTypeId& type) override;
	bool VRemoveListener(const EventListenerDelegate& eventDelegate, const EventTypeId& type) override;

	bool VTriggerEvent(const IEventDataPtr& pEvent) const override;
	bool VQueueEvent(const IEventDataPtr& pEvent) override;
	bool VUpdate() override;
	bool VAbortEvent(const EventTypeId& inType, bool allOfType) override;

	friend std::ostream& operator<<(std::ostream& os, const EventManager& mgr);
};

std::ostream& operator<<(std::ostream& os, const EventManager& mgr);