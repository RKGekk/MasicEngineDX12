#include "process_manager.h"

ProcessManager::~ProcessManager(void) {
    ClearAllProcesses();
}

unsigned int ProcessManager::UpdateProcesses(const GameTimerDelta& delta) {
    unsigned short int successCount = 0;
    unsigned short int failCount = 0;

    ProcessList::iterator it = m_processList.begin();
    while (it != m_processList.end()) {
        std::shared_ptr<Process> pCurrProcess = (*it);

        ProcessList::iterator thisIt = it;
        ++it;

        if (pCurrProcess->GetState() == Process::State::UNINITIALIZED) {
            pCurrProcess->VOnInit();
        }

        if (pCurrProcess->GetState() == Process::State::RUNNING) {
            pCurrProcess->VOnUpdate(delta);
        }

        if (pCurrProcess->IsDead()) {
            switch (pCurrProcess->GetState()) {
                case Process::State::SUCCEEDED: {
                    pCurrProcess->VOnSuccess();
                    std::shared_ptr<Process> pChild = pCurrProcess->RemoveChild();
                    if (pChild) {
                        AttachProcess(pChild);
                    }
                    else {
                        ++successCount;
                    }
                }
                break;
                case Process::State::FAILED: {
                    pCurrProcess->VOnFail();
                    ++failCount;
                }
                break;
                case Process::State::ABORTED: {
                    pCurrProcess->VOnAbort();
                    ++failCount;
                }
                break;
            }
            m_processList.erase(thisIt);
        }
    }

    return ((successCount << 16) | failCount);
}

std::weak_ptr<Process> ProcessManager::AttachProcess(std::shared_ptr<Process> pProcess) {
    m_processList.push_front(pProcess);
    return std::weak_ptr<Process>(pProcess);
}

void ProcessManager::ClearAllProcesses(void) {
    m_processList.clear();
}

void ProcessManager::AbortAllProcesses(bool immediate) {
    ProcessList::iterator it = m_processList.begin();
    while (it != m_processList.end()) {
        ProcessList::iterator tempIt = it;
        ++it;

        std::shared_ptr<Process> pProcess = *tempIt;
        if (pProcess->IsAlive()) {
            pProcess->SetState(Process::State::ABORTED);
            if (immediate) {
                pProcess->VOnAbort();
                m_processList.erase(tempIt);
            }
        }
    }
}

size_t ProcessManager::GetProcessCount() const {
    return m_processList.size();
}