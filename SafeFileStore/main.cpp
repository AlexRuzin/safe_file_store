#include <cctype>
#include <cstdint>
#include <filesystem>
#include <algorithm>
#include <cstddef>
#include <openssl/evp.h>

#include "user_logging.h"
#include "shared.h"
#include "config.h"
#include "params.h"
#include "crypto.h"
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
	ERROR err = ERROR_OK;

	if (std::filesystem::exists(val)) {
		return ERROR_FILE_ALREADY_EXISTS;
	}

	// Get password
	std::string pass;
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
	std::getline(std::cin, pass);

	// Gen sha256
	std::byte sha256[EVP_MAX_MD_SIZE];
	std::vector<std::byte> passVec;
	passVec.reserve(pass.size());
	std::transform(pass.begin(),pass.end(),
		std::back_inserter(passVec),
		[](unsigned char c) { return static_cast<std::byte>(c); });

	pass.clear();

	// Generate sha256 of pass
	err = CalculateSHA256(passVec, sha256);
	if (err != ERROR_OK) {
		return err;
	}

	// crc32 of blankfile
	std::vector<std::byte> fileData;
	fileData.reserve(baseFileContents.size());
	std::transform(baseFileContents.begin(),baseFileContents.end(),
		std::back_inserter(fileData),
		[](unsigned char c) { return static_cast<std::byte>(c); });
	
	CRC32 crc32 = CalculateCrc32(fileData);

	// Create file
	CRYPT_FILE_HEADER hdr(fileData.size(), crc32);



	return ERROR_OK;
}

static ERROR HandleOpenFile(const std::string &val) {
	const std::vector<std::byte> *buf = ReadFile(val);
	if (buf == nullptr) {
		return ERROR_CANNOT_OPEN_FILE;
	}

	return ERROR_OK;
}