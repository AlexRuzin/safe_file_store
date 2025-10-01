#pragma once

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <stdint.h>

static const char delimiter = '=';

class ParameterParser {
private:
	std::vector<std::string> input;
	std::vector<std::string> flags;
	std::map<std::string, std::string> flagVals;

public:
	ParameterParser(uint32_t argc, const char *argv[]) 
	{
		for (uint32_t i = 0; i < argc; i++) {
			input.push_back(argv[i]);
		}

		for (std::vector<std::string>::const_iterator i = input.begin();
			i != input.end(); i++) 
		{
			std::vector<std::string> split = SplitString(*i, delimiter);
			switch (split.size()) {
			case 0:
			case 1:
				continue;
			case 2:
				flagVals.insert({split[0], split[1]});
			}
		}
	}

	// Get vector of parameters
	inline std::vector<std::string> GetAllParams(void) const;

	// Get the value of a parameter which is in a key/val format like
	//  -file=filename.txt (file is key, filename.txt is val)
	inline const std::string *GetParamValue(const std::string &key) const;
};

inline std::vector<std::string> ParameterParser::GetAllParams(void) const {
	return input;
}

inline const std::string *ParameterParser::GetParamValue(const std::string& key) const {
	const std::map<std::string, std::string>::const_iterator it = flagVals.find(key);
	if (it != flagVals.end()) {
		return &(it->second);
	} 

	return nullptr;
}