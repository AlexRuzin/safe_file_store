#include <cctype>
#include <cstdint>

#include "user_logging.h"
#include "shared.h"
#include "config.h"
#include "params.h"

int32_t main(uint32_t argc, const char* argv[]) {
	// Init logging
	LOG_INIT(ProjectName, LOG_SOURCE_CONSOLE);
	LOG_INFO("%s", "test\n");

	// Parse parameters
	ParameterParser params(argc, argv);
	const std::vector<std::string> paramVec = params.GetAllParams();
	

	return 0;
}