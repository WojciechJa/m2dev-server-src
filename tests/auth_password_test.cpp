#include "auth_password.h"

#include <cstring>
#include <iostream>

#include <sodium.h>

namespace
{
	bool Expect(bool condition, const char* message)
	{
		if (!condition)
			std::cerr << "FAIL: " << message << '\n';
		return condition;
	}
}

int main()
{
	if (sodium_init() < 0)
	{
		std::cerr << "FAIL: sodium_init\n";
		return 1;
	}

	bool passed = true;
	EAccountPasswordHashAlgorithm algorithm = EAccountPasswordHashAlgorithm::UNKNOWN;

	const char* legacyHash = "*0123456789ABCDEF0123456789ABCDEF01234567";
	passed &= Expect(
		VerifyAccountPassword("ignored", legacyHash, legacyHash, &algorithm),
		"legacy MySQL candidate should match");
	passed &= Expect(
		algorithm == EAccountPasswordHashAlgorithm::LEGACY_QUERY,
		"legacy algorithm should be reported");
	passed &= Expect(
		!VerifyAccountPassword("ignored", "*BAD", legacyHash, nullptr),
		"wrong legacy candidate should fail");

	char argonHash[crypto_pwhash_STRBYTES];
	const char* argonPassword = "M2devTest9!";
	if (0 != crypto_pwhash_str(
		argonHash,
		argonPassword,
		std::strlen(argonPassword),
		crypto_pwhash_OPSLIMIT_INTERACTIVE,
		crypto_pwhash_MEMLIMIT_INTERACTIVE))
	{
		std::cerr << "FAIL: crypto_pwhash_str\n";
		return 1;
	}
	passed &= Expect(
		VerifyAccountPassword(argonPassword, nullptr, argonHash, &algorithm),
		"Argon2id hash should match");
	passed &= Expect(
		algorithm == EAccountPasswordHashAlgorithm::ARGON2,
		"Argon2 algorithm should be reported");
	passed &= Expect(
		!VerifyAccountPassword("wrong", nullptr, argonHash, nullptr),
		"wrong Argon2 password should fail");

#if defined(OS_FREEBSD) || defined(OS_LINUX) || defined(__FreeBSD__) || defined(__linux__)
	// Portable PHP-compatible bcrypt test vector for the plaintext "password".
	const char* bcryptHash = "$2y$10$92IXUNpkjO0rOQ5byMi.Ye4oKoEa3Ro9llC/.og/at2uheWG/igi.";
	passed &= Expect(
		VerifyAccountPassword("password", nullptr, bcryptHash, &algorithm),
		"PHP bcrypt hash should match");
	passed &= Expect(
		algorithm == EAccountPasswordHashAlgorithm::BCRYPT,
		"bcrypt algorithm should be reported");
	passed &= Expect(
		!VerifyAccountPassword("wrong", nullptr, bcryptHash, nullptr),
		"wrong bcrypt password should fail");
#endif

	return passed ? 0 : 1;
}
