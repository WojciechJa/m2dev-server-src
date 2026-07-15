#include "auth_password.h"

#include <cstring>
#include <mutex>
#include <string>

#include <sodium.h>

#if defined(OS_LINUX) || defined(__linux__) || defined(OS_FREEBSD) || defined(__FreeBSD__)
#include <crypt.h>
#define M2DEV_HAS_SYSTEM_CRYPT 1
#endif

namespace
{
	bool EnsureSodiumInitialized()
	{
		static const bool initialized = sodium_init() >= 0;
		return initialized;
	}

	bool HasPrefix(const char* value, const char* prefix)
	{
		return value && prefix && 0 == std::strncmp(value, prefix, std::strlen(prefix));
	}

	bool ConstantTimeEquals(const char* left, const char* right)
	{
		if (!left || !right)
			return false;

		const size_t leftLength = std::strlen(left);
		const size_t rightLength = std::strlen(right);
		return leftLength == rightLength &&
			0 == sodium_memcmp(left, right, leftLength);
	}

	bool IsBcryptHash(const char* storedHash)
	{
		return HasPrefix(storedHash, "$2a$") ||
			HasPrefix(storedHash, "$2b$") ||
			HasPrefix(storedHash, "$2y$");
	}

	bool IsArgon2Hash(const char* storedHash)
	{
		return HasPrefix(storedHash, "$argon2i$") ||
			HasPrefix(storedHash, "$argon2id$");
	}

	bool VerifyBcrypt(const char* plainPassword, const char* storedHash)
	{
#if defined(M2DEV_HAS_SYSTEM_CRYPT)
		// PHP emits $2y$, while FreeBSD crypt(3) accepts the equivalent $2b$
		// revision. Only the revision marker changes; the salt and checksum do not.
		std::string cryptHash(storedHash);
		if (HasPrefix(storedHash, "$2y$"))
			cryptHash[2] = 'b';

		// crypt() owns a process-global result buffer. Serialize access and copy no
		// secrets outside this scope. FreeBSD provides bcrypt through crypt().
		static std::mutex s_cryptMutex;
		std::lock_guard<std::mutex> lock(s_cryptMutex);
		const char* result = crypt(plainPassword, cryptHash.c_str());
		return result && ConstantTimeEquals(result, cryptHash.c_str());
#else
		(void)plainPassword;
		(void)storedHash;
		return false;
#endif
	}
}

const char* GetAccountPasswordHashAlgorithmName(EAccountPasswordHashAlgorithm algorithm)
{
	switch (algorithm)
	{
		case EAccountPasswordHashAlgorithm::LEGACY_QUERY:
			return "legacy-query";
		case EAccountPasswordHashAlgorithm::BCRYPT:
			return "bcrypt";
		case EAccountPasswordHashAlgorithm::ARGON2:
			return "argon2";
		default:
			return "unknown";
	}
}

bool VerifyAccountPassword(
	const char* plainPassword,
	const char* legacyQueryCandidate,
	const char* storedHash,
	EAccountPasswordHashAlgorithm* algorithm)
{
	EAccountPasswordHashAlgorithm detected = EAccountPasswordHashAlgorithm::UNKNOWN;
	bool valid = false;
	if (!EnsureSodiumInitialized())
	{
		if (algorithm)
			*algorithm = detected;
		return false;
	}

	if (plainPassword && storedHash && IsBcryptHash(storedHash))
	{
		detected = EAccountPasswordHashAlgorithm::BCRYPT;
		valid = VerifyBcrypt(plainPassword, storedHash);
	}
	else if (plainPassword && storedHash && IsArgon2Hash(storedHash))
	{
		detected = EAccountPasswordHashAlgorithm::ARGON2;
		valid = 0 == crypto_pwhash_str_verify(
			storedHash,
			plainPassword,
			std::strlen(plainPassword));
	}
	else if (legacyQueryCandidate && storedHash)
	{
		detected = EAccountPasswordHashAlgorithm::LEGACY_QUERY;
		valid = ConstantTimeEquals(legacyQueryCandidate, storedHash);
	}

	if (algorithm)
		*algorithm = detected;
	return valid;
}
