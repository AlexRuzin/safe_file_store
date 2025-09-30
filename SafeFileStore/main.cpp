#include <cctype>
#include <cstdint>

#include "user_logging.h"
#include "shared.h"

int32_t main(uint32_t argc, const char* argv[]) {
	LOG_INIT(ProjectName, LOG_SOURCE_CONSOLE);
	LOG_INFO("%s", "test");

	Sleep(INFINITE);

	return 0;
}