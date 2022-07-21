#include "event_manager.h"

EventManager::EventManager(const std::string& pName, bool setAsGlobal) : IEventManager(setAsGlobal), m_eventManagerName(pName) {
	m_activeQueue = 0;
}

bool EventManager::VAddListener(const EventListenerDelegate& eventDelegate, const EventTypeId& type) {
	auto& eventListenerList = m_eventListeners[type];
	for (auto it = eventListenerList.begin(); it != eventListenerList.end(); ++it) {
		if (eventDelegate == (*it)) {
			return false;
		}
	}
	eventListenerList.push_back(eventDelegate);
	return true;
}

bool EventManager::VRemoveListener(const EventListenerDelegate& eventDelegate, const EventTypeId& type) {
	bool success = false;
	auto findIt = m_eventListeners.find(type);
	if (findIt != m_eventListeners.end()) {
		auto& listeners = findIt->second;
		for (auto it = listeners.begin(); it != listeners.end(); ++it) {
			if (eventDelegate == (*it)) {
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
	auto findIt = m_eventListeners.find(pEvent->VGetEventType());
	if (findIt != m_eventListeners.end()) {
		const auto& eventListenerList = findIt->second;
		for (auto it = eventListenerList.begin(); it != eventListenerList.end(); ++it) {
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

	auto findIt = m_eventListeners.find(pEvent->VGetEventType());
	if (findIt != m_eventListeners.end()) {
		m_queues[m_activeQueue].push_back(pEvent);
		return true;
	}
	else {
		return false;
	}
}

bool EventManager::VUpdate() {
	int queueToProcess = m_activeQueue;
	m_activeQueue = (m_activeQueue + 1) % EVENTMANAGER_NUM_QUEUES;
	m_queues[m_activeQueue].clear();

	while (!m_queues[queueToProcess].empty()) {
		auto pEvent = m_queues[queueToProcess].front();
		m_queues[queueToProcess].pop_front();

		const unsigned long& eventType = pEvent->VGetEventType();

		auto findIt = m_eventListeners.find(eventType);
		if (findIt != m_eventListeners.end()) {
			const auto& eventListeners = findIt->second;

			for (auto it = eventListeners.begin(); it != eventListeners.end(); ++it) {
				auto listener = (*it);
				listener(pEvent);
			}
		}
	}

	bool queueFlushed = (m_queues[queueToProcess].empty());
	if (!queueFlushed) {
		while (!m_queues[queueToProcess].empty()) {
			auto pEvent = m_queues[queueToProcess].back();
			m_queues[queueToProcess].pop_back();
			m_queues[m_activeQueue].push_front(pEvent);
		}
	}

	return queueFlushed;
}

bool EventManager::VAbortEvent(const EventTypeId& inType, bool allOfType) {
	bool success = false;
	auto findIt = m_eventListeners.find(inType);

	if (findIt != m_eventListeners.end()) {
		auto& eventQueue = m_queues[m_activeQueue];
		auto it = eventQueue.begin();
		while (it != eventQueue.end()) {
			auto thisIt = it;
			++it;

			if ((*thisIt)->VGetEventType() == inType) {
				eventQueue.erase(thisIt);
				success = true;
				if (!allOfType)
					break;
			}
		}
	}

	return success;
}

std::ostream& operator<<(std::ostream& os, const EventManager& mgr) {
	std::ios::fmtflags oldFlag = os.flags();

	std::cout << "EventManager name: " << mgr.m_eventManagerName << std::endl;
	std::cout << "Contains listeners:" << std::endl;
	int counter = 0;
	for (const auto& [eventTypeId, listenerFx] : mgr.m_eventListeners) {
		std::cout << ++counter << ") Listener for event type id: " << eventTypeId << " with name: " << GET_EVENT_NAME(eventTypeId) << std::endl;
	}
	std::cout << "Current active queue: " << mgr.m_activeQueue << std::endl;
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