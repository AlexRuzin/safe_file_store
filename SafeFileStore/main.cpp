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

	// Parse parameters
	ParameterParser params(argc, argv);
	const std::vector<std::string> paramVec = params.GetAllParams();

	// Open existing file
	if (params.GetParamValue(FlagOpen) != nullptr) {
		const std::string *path = params.GetParamValue(FlagOpen);
		if (path == nullptr) {
			return ERROR_CANNOT_OPEN_FILE;
		}
		LOG_INFO("opening existing file: %s", path->c_str());
		const ERROR err = HandleOpenFile(*path);
		if (err != ERROR_OK) {
			LOG_ERROR("HandleOpenFile failed with 0x%08x\n", err);
			return err;
		}

		LOG_INFO("cleanly closing existing file: %s",path->c_str());
		return 0;
	}

	// Create new file
	if (params.GetParamValue(FlagCreate) != nullptr) {
		const std::string* path = params.GetParamValue(FlagCreate);
		if(path == nullptr) {
			return ERROR_CANNOT_CREATE_FILE;
		}
		LOG_INFO("opening new file: %s",path->c_str());
		const ERROR err = HandleCreateFile(*path);
		if(err != ERROR_OK) {
			LOG_ERROR("HandleCraeteFile failed with 0x%08x\n", err);
			return err;
		}

		LOG_INFO("cleanly closing new file: %s", path->c_str());
		return 0;
	}

	// Test operations
	if(params.GetParamValue(FlagTest) != nullptr) {
		LOG_DEBUG("entering crypto test mode...");
		const std::string* path = params.GetParamValue(FlagCreate);
		if(path == nullptr) {
			LOG_ERROR("flag %s must have a parameter", FlagTest.c_str());
			return ERROR_CANNOT_CREATE_FILE;
		}

		// Delete test file if exists
		if(std::filesystem::exists(*path)) {
			if(std::filesystem::remove(*path)) {
				LOG_ERROR("failed to delete test file %s", path->c_str());
				return ERROR_DELETE_FILE_FAIL;
			}
		}

		// Create new file
		LOG_INFO("opening new file: %s",path->c_str());
		ERROR err = HandleCreateFile(*path);
		if(err != ERROR_OK) {
			LOG_ERROR("HandleCraeteFile failed with 0x%08x\n",err);
			return err;
		}

		err = HandleOpenFile(*path);
		if(err != ERROR_OK) {
			LOG_ERROR("HandleOpenFile failed with 0x%08x\n",err);
			return err;
		}

		LOG_INFO("test successful on file: %s",path->c_str());
		return 0;
	}


	LOG_INFO("no parameters specified, closing\n");
	return 0;
}

static ERROR HandleCreateFile(const std::string &val) {
	ERROR err = ERROR_OK;

	if (std::filesystem::exists(val)) {
		LOG_WARNING("File exists (-create flag) %s. Deleting.", val.c_str());
		if(std::filesystem::remove(val)) {
			return ERROR_DELETE_FILE_FAIL;
		}
	}

	// Get password
	std::string pass;
	//std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
	std::getline(std::cin, pass);

	// Gen sha256
	std::vector<std::byte> sha256(EVP_MAX_MD_SIZE / 2);
	std::vector<std::byte> passVec;
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
	std::transform(baseFileContents.begin(),baseFileContents.end(),
		std::back_inserter(fileData),
		[](unsigned char c) { return static_cast<std::byte>(c); });
	
	CRC32 crc32 = CalculateCrc32(fileData);

	// Create file
	CRYPT_FILE_HEADER hdr(fileData.size(), crc32);

	// Create ciphertext
	std::vector<std::byte> iv(EVP_MAX_IV_LENGTH);
	std::vector<std::byte> cipherText;
	err = EncryptData(fileData, sha256, iv, &cipherText);
	if (err != ERROR_OK) {
		return err;
	}

	// Create file
	std::vector<std::byte> rawFile(sizeof(CRYPT_FILE_HEADER) + cipherText.size());
	std::memcpy(rawFile.data(), &hdr, sizeof(CRYPT_FILE_HEADER));
	std::memcpy(rawFile.data() + sizeof(CRYPT_FILE_HEADER), cipherText.data(), cipherText.size());
	err = WriteFile(rawFile, val);
	if (err != ERROR_OK) {
		return err;
	}

	return ERROR_OK;
}

static ERROR HandleOpenFile(const std::string &val) {
	const std::vector<std::byte> *buf = ReadFile(val);
	if (buf == nullptr) {
		return ERROR_CANNOT_OPEN_FILE;
	}

	return ERROR_OK;
}