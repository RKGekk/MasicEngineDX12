#include "event_manager.h"

EventManager::EventManager(const std::string& pName, bool setAsGlobal) : IEventManager(setAsGlobal), m_event_manager_name(pName) {
	m_active_queue = 0;
}

bool EventManager::VAddListener(const EventListenerDelegate& event_delegate, const EventTypeId& type) {
	auto& event_listener_list = m_event_listeners[type];
	for (auto it = event_listener_list.begin(); it != event_listener_list.end(); ++it) {
		if (event_delegate == (*it)) {
			return false;
		}
	}
	event_listener_list.push_back(event_delegate);
	return true;
}

bool EventManager::VRemoveListener(const EventListenerDelegate& event_delegate, const EventTypeId& type) {
	bool success = false;
	auto findIt = m_event_listeners.find(type);
	if (findIt != m_event_listeners.end()) {
		auto& listeners = findIt->second;
		for (auto it = listeners.begin(); it != listeners.end(); ++it) {
			if (event_delegate == (*it)) {
				listeners.erase(it);
				success = true;
				break;
			}
		}
	}
	return success;
}

bool EventManager::VTriggerEvent(const IEventDataPtr& pEvent) const {
	bool processed = false;
	auto find_it = m_event_listeners.find(pEvent->VGetEventType());
	if (find_it != m_event_listeners.end()) {
		const auto& event_listener_list = find_it->second;
		for (auto it = event_listener_list.begin(); it != event_listener_list.end(); ++it) {
			auto listener = (*it);
			listener(pEvent);
			processed = true;
		}
	}
	return processed;
}

bool EventManager::VQueueEvent(const IEventDataPtr& pEvent) {
	if (!pEvent) {
		return false;
	}

	auto findIt = m_event_listeners.find(pEvent->VGetEventType());
	if (findIt != m_event_listeners.end()) {
		m_queues[m_active_queue].push_back(pEvent);
		return true;
	}
	else {
		return false;
	}
}

bool EventManager::VThreadSafeQueueEvent(const IEventDataPtr& pEvent) {
	m_realtime_event_queue.Push(pEvent);
	return true;
}

bool EventManager::VUpdate() {
	GameTimePoint curr_ns = GameClock::now();
	GameTimePoint max_ns = curr_ns + MAX_DURATION;

	IEventDataPtr pRealtime_event;
	while (m_realtime_event_queue.TryPop(pRealtime_event)) {
		VQueueEvent(pRealtime_event);
		//if (GameClock::now() >= max_ns) { throw("A realtime process is spamming the event manager!"); }
	}

	int queue_to_process = m_active_queue;
	m_active_queue = (m_active_queue + 1) % EVENTMANAGER_NUM_QUEUES;
	m_queues[m_active_queue].clear();

	while (!m_queues[queue_to_process].empty()) {
		auto pEvent = m_queues[queue_to_process].front();
		m_queues[queue_to_process].pop_front();

		const unsigned long& event_type = pEvent->VGetEventType();

		auto findIt = m_event_listeners.find(event_type);
		if (findIt != m_event_listeners.end()) {
			const auto& event_listeners = findIt->second;

			for (auto it = event_listeners.begin(); it != event_listeners.end(); ++it) {
				auto listener = (*it);
				listener(pEvent);
			}
		}
	}

	bool queue_flushed = (m_queues[queue_to_process].empty());
	if (!queue_flushed) {
		while (!m_queues[queue_to_process].empty()) {
			auto pEvent = m_queues[queue_to_process].back();
			m_queues[queue_to_process].pop_back();
			m_queues[m_active_queue].push_front(pEvent);
		}
	}

	return queue_flushed;
}

bool EventManager::VAbortEvent(const EventTypeId& in_type, bool all_of_type) {
	bool success = false;
	auto findIt = m_event_listeners.find(in_type);

	if (findIt != m_event_listeners.end()) {
		auto& event_queue = m_queues[m_active_queue];
		auto it = event_queue.begin();
		while (it != event_queue.end()) {
			auto this_it = it;
			++it;

			if ((*this_it)->VGetEventType() == in_type) {
				event_queue.erase(this_it);
				success = true;
				if (!all_of_type)
					break;
			}
		}
	}

	return success;
}

std::ostream& operator<<(std::ostream& os, const EventManager& mgr) {
	std::ios::fmtflags oldFlag = os.flags();

	std::cout << "EventManager name: " << mgr.m_event_manager_name << std::endl;
	std::cout << "Contains listeners:" << std::endl;
	int counter = 0;
	for (const auto& [eventTypeId, listenerFx] : mgr.m_event_listeners) {
		std::cout << ++counter << ") Listener for event type id: " << eventTypeId << " with name: " << GET_EVENT_NAME(eventTypeId) << std::endl;
	}
	std::cout << "Current active queue: " << mgr.m_active_queue << std::endl;
	std::cout << "Events queue contains:" << std::endl;
	int queueCounter = 0;
	int eventCounter = 0;
	for (const auto& currentQueue : mgr.m_queues) {
		std::cout << queueCounter++ << ") queue ->" << std::endl;
		for (const auto& currentEvent : currentQueue) {
			std::cout << "\t" << ++eventCounter << ") event id: " << currentEvent->VGetEventType() << " with name: " << currentEvent->GetName() << std::endl;
		}
	}

	os.flags(oldFlag);
	return os;
}