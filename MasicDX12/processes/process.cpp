#include "process.h"

Process::Process() {
	m_state = State::UNINITIALIZED;
}

Process::~Process() {
	if (m_pChild) {
		m_pChild->VOnAbort();
	}
}

std::shared_ptr<Process> Process::RemoveChild() {
	if (m_pChild) {
		std::shared_ptr<Process> pChild = m_pChild;
		m_pChild.reset();
		return pChild;
	}

	return std::shared_ptr<Process>();
}

std::shared_ptr<Process> Process::PeekChild() {
	return m_pChild;
}

void Process::VOnInit() {
	m_state = State::RUNNING;
}

void Process::VOnSuccess() {}

void Process::VOnFail() {}

void Process::VOnAbort() {}

void Process::SetState(State newState) {
	m_state = newState;
}

void Process::Succeed() {
	m_state = State::SUCCEEDED;
}

void Process::Fail() {
	m_state = State::FAILED;
}

void Process::AttachChild(std::shared_ptr<Process> pChild) {
	if (m_pChild)
		m_pChild->AttachChild(pChild);
	else
		m_pChild = pChild;
}

void Process::Pause() {
	if (m_state == State::RUNNING)
		m_state = State::PAUSED;
}

void Process::UnPause() {
	if (m_state == State::PAUSED)
		m_state = State::RUNNING;
}

Process::State Process::GetState() const {
	return m_state;
}

bool Process::IsAlive() const {
	return (m_state == State::RUNNING || m_state == State::PAUSED);
}

bool Process::IsDead() const {
	return (m_state == State::SUCCEEDED || m_state == State::FAILED || m_state == State::ABORTED);
}

bool Process::IsRemoved() const {
	return (m_state == State::REMOVED);
}

bool Process::IsPaused() const {
	return m_state == State::PAUSED;
}