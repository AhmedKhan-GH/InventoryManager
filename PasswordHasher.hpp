#ifndef PASSWORDHASHER_HPP
#define PASSWORDHASHER_HPP

#include "cryptopp/cryptlib.h"
#include "cryptopp/sha.h"
#include "cryptopp/hex.h"
#include "cryptopp/osrng.h"
#include <string>

class PasswordHasher {
private:
    static const int SALT_SIZE = 16;

    std::string generateSalt() const {
        CryptoPP::AutoSeededRandomPool rng;
        CryptoPP::byte salt[SALT_SIZE];
        rng.GenerateBlock(salt, sizeof(salt));

        CryptoPP::HexEncoder encoder;
        std::string salt_hex;
        encoder.Attach(new CryptoPP::StringSink(salt_hex));
        encoder.Put(salt, sizeof(salt));
        encoder.MessageEnd();

        return salt_hex;
    }

public:
    std::string hashPassword(const std::string& password) const {
        std::string salt = generateSalt();
        std::string salted_password = salt + password;

        CryptoPP::SHA256 hash;
        CryptoPP::byte digest[CryptoPP::SHA256::DIGESTSIZE];
        hash.CalculateDigest(digest, reinterpret_cast<const CryptoPP::byte*>(salted_password.data()), salted_password.size());

        CryptoPP::HexEncoder encoder;
        std::string hash_hex;
        encoder.Attach(new CryptoPP::StringSink(hash_hex));
        encoder.Put(digest, sizeof(digest));
        encoder.MessageEnd();

        return salt + ":" + hash_hex;
    }

    bool validatePassword(const std::string& stored_hash, const std::string& password) const {
        size_t delimiter_pos = stored_hash.find(':');
        if (delimiter_pos == std::string::npos) {
            return false;
        }

        std::string salt = stored_hash.substr(0, delimiter_pos);
        std::string salted_password = salt + password;

        CryptoPP::SHA256 hash;
        CryptoPP::byte digest[CryptoPP::SHA256::DIGESTSIZE];
        hash.CalculateDigest(digest, reinterpret_cast<const CryptoPP::byte*>(salted_password.data()), salted_password.size());

        CryptoPP::HexEncoder encoder;
        std::string new_hash_hex;
        encoder.Attach(new CryptoPP::StringSink(new_hash_hex));
        encoder.Put(digest, sizeof(digest));
        encoder.MessageEnd();

        return (salt + ":" + new_hash_hex) == stored_hash;
    }
};

#endif PASSWORDHASHER_HPP