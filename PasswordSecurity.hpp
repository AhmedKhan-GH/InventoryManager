#ifndef PASSWORDSECURITY_HPP
#define PASSWORDSECURITY_HPP

#include <string>
#include <random>
#include "SHA1.hpp"

/// <summary>
/// handles basic salt generation, password hashing
/// and password validation with an existing hash
/// 
/// @author: Ahmed Khan
/// @date: 11/25/23
/// </summary>
class PasswordSecurity {
public:
    /// <summary>
    /// generates a randomized string of specified length
    /// </summary>
    /// <param name="length">specification of length, default 8</param>
    /// <returns>returns a string</returns>
    static std::string generate_salt(size_t length = 8) 
    {
        const char* alphanum =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

        std::string salt;
        std::mt19937 generator(std::random_device{}());
        std::uniform_int_distribution<size_t> distribution(0, strlen(alphanum) - 1);

        for (size_t i = 0; i < length; ++i){salt += alphanum[distribution(generator)];}
        return salt;
    }
    
    /// <summary>
    /// performs a SHA1 encryption of a given password salt combination
    /// </summary>
    /// <param name="string">the string to be hashed</param>
    /// <returns>a 40 char hash</returns>
    static std::string hash_password(const std::string& password, const std::string& salt) {
        return SHA1::hash(password + salt);
    }

    /// <summary>
    /// validates a given entry and its respective salt to a hash
    /// </summary>
    /// <param name="hashed"></param>
    /// <param name="password"></param>
    /// <param name="salt"></param>
    /// <returns></returns>
    static bool validate_password(const std::string& hashed, const std::string& password, const std::string& salt) {
        if(hashed == hash_password(password, salt)){return true;}
        else{return false;}
    }
};

#endif //PASSWORDSECURITY_HPP