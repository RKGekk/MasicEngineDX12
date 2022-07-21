#pragma once

#include <vector>
#include <string>

typedef std::string Level;

class LevelManager {
public:
	const std::vector<Level>& GetLevels() const;
	const int GetCurrentLevel() const;
	bool Initialize(std::vector<std::string>& levels);
	bool Initialize();

protected:
	std::vector<Level> m_Levels;
	int m_CurrentLevel;
};