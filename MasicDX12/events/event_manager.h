#pragma once

#include <iostream>
#include <unordered_map>
#include <list>
#include <string>

#include "i_event_manager.h"
#include "../tools/thread_safe_queue.h"
#include "../tools/game_timer.h"

const unsigned int EVENTMANAGER_NUM_QUEUES = 2;
const GameClockDuration MAX_DURATION = std::chrono::milliseconds(33);

class EventManager : public IEventManager {
	using EventListenerMap = std::unordered_map<EventTypeId, std::list<EventListenerDelegate>>;
	using EventListenerList = std::list<IEventDataPtr>;
	using ThreadSafeEventQueue = ThreadSafeQueue<IEventDataPtr>;

	EventListenerMap m_event_listeners;
	EventListenerList m_queues[EVENTMANAGER_NUM_QUEUES];
	ThreadSafeEventQueue m_realtime_event_queue;
	int m_active_queue;
	const std::string m_event_manager_name;

public:
	explicit EventManager(const std::string& pName, bool setAsGlobal);

	bool VAddListener(const EventListenerDelegate& eventDelegate, const EventTypeId& type) override;
	bool VRemoveListener(const EventListenerDelegate& eventDelegate, const EventTypeId& type) override;

	bool VTriggerEvent(const IEventDataPtr& pEvent) const override;
	bool VQueueEvent(const IEventDataPtr& pEvent) override;
	bool VThreadSafeQueueEvent(const IEventDataPtr& pEvent);
	bool VUpdate() override;
	bool VAbortEvent(const EventTypeId& inType, bool allOfType) override;

	friend std::ostream& operator<<(std::ostream& os, const EventManager& mgr);
};

std::ostream& operator<<(std::ostream& os, const EventManager& mgr);