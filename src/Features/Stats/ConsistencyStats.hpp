#pragma once
#include "Command.hpp"
#include "Features.hpp"
#include "Modules/Engine.hpp"
#include "Modules/Server.hpp"
#include "Modules/Console.hpp"
#include <filesystem>
#include <string>

#define SAR_MTRIGGER_SPLITS_HEADER "Splitname, Times"


class ConsistencyStats : public Feature{
private:
	std::unordered_map<std::string, std::vector<float>> mtriggerTimes;
public:
	//void Reset();
	bool ImportFromFile(const std::string &path);
	bool ExportToFile(const std::string &path);
	void Print();
	void RecordData(std::string, float);
};

extern ConsistencyStats *consistencyStats;
extern Variable sar_consistencystats_enable;