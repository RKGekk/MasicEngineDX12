#include "exec_process.h"

ExecProcess::ExecProcess(std::function<bool()> fn) : m_fn(std::move(fn)) {}

void ExecProcess::VOnUpdate(float deltaMs) {
	if (m_fn()) {
		Succeed();
	}
	else {
		Fail();
	}
}