#pragma once

enum class EAccountPasswordHashAlgorithm
{
	LEGACY_QUERY,
	BCRYPT,
	ARGON2,
	UNKNOWN,
};

const char* GetAccountPasswordHashAlgorithmName(EAccountPasswordHashAlgorithm algorithm);

bool VerifyAccountPassword(
	const char* plainPassword,
	const char* legacyQueryCandidate,
	const char* storedHash,
	EAccountPasswordHashAlgorithm* algorithm = nullptr);
