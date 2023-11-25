#include <string>
#include <random>
#include "SHA1.hpp"

class PasswordSecurity {
public:
    static std::string generateSalt(size_t length = 16) {
        const char* alphanum =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

        std::string salt;
        std::mt19937 generator(std::random_device{}());
        std::uniform_int_distribution<size_t> distribution(0, strlen(alphanum) - 1);

        for (size_t i = 0; i < length; ++i) {
            salt += alphanum[distribution(generator)];
        }

        return salt;
    }

    static std::string hashPassword(const std::string& password, const std::string& salt) {
        return SHA1::hash(salt + password);
    }

    static bool validatePassword(const std::string& hashed, const std::string& password, const std::string& salt) {
        return hashed == hashPassword(password, salt);
    }
};
