#include "PasswordSecurity.hpp"
#include <iostream>

int main()
{
    std::string correct_password = "TestPassword";
    std::cout << "Predefined correct test password: " << correct_password << std::endl;

    std::string incorrect_password = "PasswordTest";
    std::cout << "Predefined incorrect test password: " << incorrect_password << std::endl;

    std::string unsalted_hash = PasswordSecurity::hash_password(correct_password, "");
    std::cout << "Unsalted password after hash: " << unsalted_hash << std::endl;

    std::string salt = PasswordSecurity::generate_salt();
    std::cout << "Randomly generated salt: " << salt << std::endl;

    std::cout << "Salted password before hash: " << correct_password << salt << std::endl;
    
    std::string salted_hash = PasswordSecurity::hash_password(correct_password, salt);
    std::cout << "Salted password after hash: " << salted_hash << std::endl;

    std::cout << "Password verification with correct password + salt: " << correct_password << salt << std::endl;
    
    if (PasswordSecurity::validate_password(salted_hash, correct_password, salt))
    {
        std::cout << "This password is correct" << std::endl;
    }
    else
    {
        std::cout << "This password is incorrect" << std::endl;
    }

    std::cout << "Password verification with incorrect password + salt: " << incorrect_password << salt << std::endl;

    if (PasswordSecurity::validate_password(salted_hash, incorrect_password, salt))
    {
        std::cout << "This password is correct" << std::endl;
    }
    else
    {
        std::cout << "This password is incorrect" << std::endl;
    }
}