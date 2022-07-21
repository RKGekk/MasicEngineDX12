#pragma once

#include "../tools/generic_object_factory.h"
#include "i_event_data.h"

extern GenericObjectFactory<IEventData, EventTypeId> g_eventFactory;

#define REGISTER_EVENT(eventClass) g_eventFactory.Register<eventClass>(eventClass::sk_EventType, eventClass::sk_EventName)
#define CREATE_EVENT(eventType) g_eventFactory.Create(eventType)
#define GET_EVENT_NAME(eventType) g_eventFactory.GetName(eventType)

class IEventManager {
public:

	explicit IEventManager(bool setAsGlobal);
	virtual ~IEventManager();

	virtual bool VAddListener(const EventListenerDelegate& eventDelegate, const EventTypeId& type) = 0;
	virtual bool VRemoveListener(const EventListenerDelegate& eventDelegate, const EventTypeId& type) = 0;
	virtual bool VTriggerEvent(const IEventDataPtr& pEvent) const = 0;
	virtual bool VQueueEvent(const IEventDataPtr& pEvent) = 0;
	virtual bool VUpdate() = 0;
	virtual bool VAbortEvent(const EventTypeId& type, bool allOfType = false) = 0;

	static IEventManager* Get();
	static IEventDataPtr Create(EventTypeId eventType);
};