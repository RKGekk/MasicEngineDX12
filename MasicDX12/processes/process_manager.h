#pragma once

#include <list>

#include "process.h"
#include "../tools/game_timer.h"

class ProcessManager {
	typedef std::list<std::shared_ptr<Process>> ProcessList;

	ProcessList m_processList;

public:
	~ProcessManager();

	unsigned int UpdateProcesses(const GameTimerDelta& delta);
	std::weak_ptr<Process> AttachProcess(std::shared_ptr<Process> pProcess);
	void AbortAllProcesses(bool immediate);

	size_t GetProcessCount() const;

private:
	void ClearAllProcesses();
};