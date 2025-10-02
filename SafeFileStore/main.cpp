#include <cctype>
#include <cstdint>
#include <fstream>
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

ERROR HandleCreateFile(const std::string &path);
ERROR HandleOpenFile(const std::string &path);

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
		const std::string* path = params.GetParamValue(FlagTest);
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

static ERROR HandleCreateFile(const std::string &path) {
	ERROR err = ERROR_OK;

	FileOps file(path);
	err = file.LockFile();
	if (err != ERROR_OK) {
		return err;
	}

	const std::vector<std::byte> iv(EVP_MAX_IV_LENGTH);
	CryptOps crypt(CryptOps::GetPass(), iv);

	// crc32 of blankfile
	std::vector<std::byte> fileData;
	std::transform(baseFileContents.begin(),baseFileContents.end(),
		std::back_inserter(fileData),
		[](unsigned char c) { return static_cast<std::byte>(c); });
	
	CRC32 crc32 = CryptOps::CalculateCrc32(fileData);

	// header
	CRYPT_FILE_HEADER hdr(fileData.size(), crc32);

	// ciphertext
	std::vector<std::byte> *cipherText = nullptr;
	err = crypt.EncryptData(fileData, cipherText);
	if (err != ERROR_OK) {
		return err;
	}
	if (cipherText == nullptr) {
		return ERROR_AES_ENCRYPT_FAIL;
	}

	// Create raw file data
	std::vector<std::byte> rawFile(sizeof(CRYPT_FILE_HEADER) + cipherText->size());
	std::memcpy(rawFile.data(), &hdr, sizeof(CRYPT_FILE_HEADER));
	std::memcpy(rawFile.data() + sizeof(CRYPT_FILE_HEADER), cipherText->data(), cipherText->size());

	// Write
	file.SetData(rawFile);
	err = file.WriteFile();
	if (err != ERROR_OK) {
		delete cipherText;
		return err;
	}

	delete cipherText;
	return ERROR_OK;
}

static ERROR HandleOpenFile(const std::string &path) {
	ERROR err = ERROR_OK;

	FileOps file(path);
	err = file.LockFile();
	if(err != ERROR_OK) {
		return err;
	}

	return ERROR_OK;
}