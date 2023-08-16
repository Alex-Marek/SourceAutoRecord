#include "ConsistencyStats.hpp"

ConsistencyStats *consistencyStats;

Variable sar_consistencystats_filePath("sar_consistencystats_filePath", "Stats/ConsistencyStats", "Path to the statcounter .csv file.\n", 0);

Variable sar_consistencystats_enable("sar_consistencyStats_enable", "0", "Set 1 to record mtrigger splits Set to 0 to not record mtrigger splits\n");

void ConsistencyStats::Print() {
	if (this->mtriggerTimes.size() > 0) {
		for (auto mapStat : this->mtriggerTimes) {
			console->Print("Split: %s \n", mapStat.first.c_str());
			for (auto mapTimes : mapStat.second) {
				console->Print("   :%s \n", std::to_string(mapTimes).c_str());
			}
		}
	} else {
		console->Print("Nothing to print from current session! \n");
	}
}
	bool ConsistencyStats::ImportFromFile(const std::string &path) {
		std::string filePath = std::string(engine->GetGameDirectory()) + std::string("/") + path;
		if (filePath.substr(filePath.length() - 4, 4) != ".csv")
			filePath += ".csv";

		std::ifstream file(filePath, std::ios::in);
		if (!file.good()) {
			file.close();
			return false;
		}

		std::string line;
		while (std::getline(file, line)) {
			if (line == "sep=," || line == "Splitname, Times") {
				continue;
			}
			std::stringstream ss(line);
			std::string substr;
			std::getline(ss, substr, ',');
			std::string splitName = substr;
			while (ss.good()) {	
				std::getline(ss, substr, ',');
				if (substr != "") {
					ConsistencyStats::RecordData(splitName, std::stof(substr));
				}
			}
		}
		return true;
	}


	bool ConsistencyStats::ExportToFile(const std::string& path) {
		if (this->mtriggerTimes.size() == 0) {
			console->Print("Nothing to export!");
			return false;
		}
		std::string filePath = std::string(engine->GetGameDirectory()) + std::string("/") + path;
		if (filePath.substr(filePath.length() - 4, 4) != ".csv")
			filePath += ".csv";

		if (std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath)) {
			ConsistencyStats::ImportFromFile(path);
			std::fstream file(filePath, std::ios::in | std::ios::out | std::ios::trunc);
			if (!file.good()) {
				file.close();
				return false;
			}
			file << MICROSOFT_PLEASE_FIX_YOUR_SOFTWARE_SMHMYHEAD << std::endl;
			file << SAR_MTRIGGER_SPLITS_HEADER << std::endl;
			for (auto mapStat : this->mtriggerTimes) {
				file << mapStat.first << ",";
				for (auto mapTimes : mapStat.second) {
					file << std::to_string(mapTimes) << ",";
				}
				this->mtriggerTimes.erase(mapStat.first);
				file << std::endl;
			}
			file.close();
		} else {
			std::ofstream file(filePath, std::ios::in | std::ios::out | std::ios::app);
			if (!file.good()) {
				file.close();
				return false;
			}
			file << MICROSOFT_PLEASE_FIX_YOUR_SOFTWARE_SMHMYHEAD << std::endl;
			file << SAR_MTRIGGER_SPLITS_HEADER << std::endl;

			for (auto mapStat : this->mtriggerTimes) {
				file << mapStat.first << ",";
				for (auto mapTimes : mapStat.second) {
					file << std::to_string(mapTimes) << ",";
				}
				this->mtriggerTimes.erase(mapStat.first);
				file << std::endl;
			}
			file.close();
		}
		return true;
}

void ConsistencyStats::RecordData(std::string name, float time) {
	std::vector<float> values = {};
	this->mtriggerTimes.emplace(name, values);
	this->mtriggerTimes[name].push_back(time);
}

CON_COMMAND(sar_print_consistency_stats, "Prints all consistency stats from this sesssion.\n") {
	consistencyStats->Print();
}

CON_COMMAND(sar_export_consistency_stats, "sar_export_consistency_stats <filepath> -  export the consistency stats to the specifed path in a .csv file\n") {
	bool result = false;
	std::string path = args.ArgC() == 1 ? sar_consistencystats_filePath.GetString() : args[1];

	if (args.ArgC() >= 1) {
		result = consistencyStats->ExportToFile(path);
	} else {
		return console->Print(sar_export_consistency_stats.ThisPtr()->m_pszHelpString);
	}

	if (!result) {
		return console->Print("Couldn't write to this path. Please verify you actually can write here or the folder exits: \"%s\".\n", path.c_str());
	}

	console->Print("Datas has been successfully exported.\n");
}

CON_COMMAND(sar_import_consistency_stats, "sar_import_consistency_stats <filepath> -  import the consistency stats to the specifed path in a .csv file\n") {
	bool result = false;
	std::string path = args.ArgC() == 1 ? sar_consistencystats_filePath.GetString() : args[1];

	if (args.ArgC() >= 1) {
		result = consistencyStats->ImportFromFile(path);
	} else {
		return console->Print(sar_import_consistency_stats.ThisPtr()->m_pszHelpString);
	}

	if (!result) {
		return console->Print("Couldn't write to this path. Please verify you actually can write here or the folder exits: \"%s\".\n", path.c_str());
	}

	console->Print("Datas has been successfully exported.\n");
}