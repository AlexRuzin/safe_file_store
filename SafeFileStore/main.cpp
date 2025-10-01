#include <cctype>
#include <cstdint>

#include "user_logging.h"
#include "shared.h"
#include "config.h"
#include "params.h"
#include "file.h"
#include "error.h"

ERROR HandleCreateFile(const std::string &val);
ERROR HandleOpenFile(const std::string &val);

int32_t main(uint32_t argc, const char* argv[]) {
	// Init logging
	LOG_INIT(ProjectName, LOG_SOURCE_CONSOLE);
	LOG_INFO("%s", "test\n");

	// Parse parameters
	ParameterParser params(argc, argv);
	const std::vector<std::string> paramVec = params.GetAllParams();

	// Open existing file
	if (params.GetParamValue(FlagOpen) != nullptr) {
		const std::string *path = params.GetParamValue(FlagOpen);
		if (path == nullptr) {
			return ERROR_CANNOT_OPEN_FILE;
		}
		const ERROR err = HandleOpenFile(*path);
		if (err != ERROR_OK) {
			LOG_ERROR("HandleOpenFile failed with 0x%08x\n", err);
			return err;
		}
	}

	// Create new file
	if (params.GetParamValue(FlagCreate) != nullptr) {
		const std::string* path = params.GetParamValue(FlagCreate);
		if(path == nullptr) {
			return ERROR_CANNOT_CREATE_FILE;
		}
		const ERROR err = HandleCreateFile(*path);
		if(err != ERROR_OK) {
			LOG_ERROR("HandleCraeteFile failed with 0x%08x\n", err);
			return err;
		}
	}

	LOG_INFO("clean exit\n");
	return 0;
}

static ERROR HandleCreateFile(const std::string &val) {


	return ERROR_OK;
}

static ERROR HandleOpenFile(const std::string &val) {
	const std::vector<BYTE> *buf = ReadFile(val);
	if (buf == nullptr) {
		return ERROR_CANNOT_OPEN_FILE;
	}

	return ERROR_OK;
}