#include "level_manager.h"

const std::vector<Level>& LevelManager::GetLevels() const {
	return m_Levels;
}

const int LevelManager::GetCurrentLevel() const {
	return m_CurrentLevel;
}

bool LevelManager::Initialize(std::vector<std::string>& levels) {
	return false;
}

bool LevelManager::Initialize() {
	return false;
}