#pragma once

#include <unordered_map>
#include <string>

using namespace std::literals;

template <class BaseClass, class IdType>
class GenericObjectFactory {
	std::unordered_map<IdType, BaseClass* (*)()> m_creationFunctions;
	std::unordered_map<IdType, std::string> m_names;

public:
	inline static const std::string NONAME = "No name or not registered"s;

	template <class SubClass>
	bool Register(IdType id) {
		auto findIt = m_creationFunctions.find(id);
		if (findIt == m_creationFunctions.end()) {
			m_creationFunctions[id] = []()->BaseClass* { return new SubClass; };
			return true;
		}

		return false;
	}

	template <class SubClass>
	bool Register(IdType id, const std::string& name) {
		auto findIt = m_creationFunctions.find(id);
		if (findIt == m_creationFunctions.end()) {
			m_creationFunctions[id] = []()->BaseClass* { return new SubClass; };
			m_names[id] = name;
			return true;
		}

		return false;
	}

	BaseClass* Create(IdType id) {
		auto findIt = m_creationFunctions.find(id);
		if (findIt != m_creationFunctions.end()) {
			auto pFunc = findIt->second;
			return pFunc();
		}

		return nullptr;
	}

	const std::string& GetName(IdType id) {
		auto findIt = m_names.find(id);
		if (findIt != m_names.end()) {
			return findIt->second;
		}
		return NONAME;
	}
};