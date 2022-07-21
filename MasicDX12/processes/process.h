#pragma once

#include <memory>

class Process;
typedef std::shared_ptr<Process> StrongProcessPtr;
typedef std::weak_ptr<Process> WeakProcessPtr;

class Process {
public:
	enum class State {
		UNINITIALIZED = 0,
		REMOVED,
		RUNNING,
		PAUSED,
		SUCCEEDED,
		FAILED,
		ABORTED
	};

	Process();
	virtual ~Process();

	void Succeed();
	void Fail();

	void Pause();
	void UnPause();

	State GetState() const;
	bool IsAlive() const;
	bool IsDead() const;
	bool IsRemoved() const;
	bool IsPaused() const;

	void AttachChild(std::shared_ptr<Process> pChild);
	std::shared_ptr<Process> RemoveChild();
	std::shared_ptr<Process> PeekChild();

protected:
	virtual void VOnInit();
	virtual void VOnUpdate(float deltaMs) = 0;
	virtual void VOnSuccess();
	virtual void VOnFail();
	virtual void VOnAbort();

private:
	void SetState(State newState);

	State m_state;
	std::shared_ptr<Process> m_pChild;

	friend class ProcessManager;
};