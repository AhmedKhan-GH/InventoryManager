#include <iostream>
#include <string>
#include <sstream>
#include <vector>

class SHA1 {
public:
    static std::string hash(const std::string& input) {
        std::vector<uint32_t> hashBuffer = {
            0x67452301,
            0xEFCDAB89,
            0x98BADCFE,
            0x10325476,
            0xC3D2E1F0
        };

        std::string padded = pad(input);
        processChunks(padded, hashBuffer);

        std::stringstream ss;
        for (uint32_t i : hashBuffer) {
            ss << std::hex << i;
        }

        return ss.str();
    }

private:
    static std::string pad(const std::string& input) {
        uint64_t bitLength = input.length() * 8;
        std::string padded = input + '\x80';

        while ((padded.length() * 8) % 512 != 448) {
            padded += '\x00';
        }

        for (int i = 7; i >= 0; --i) {
            padded += static_cast<char>((bitLength >> (i * 8)) & 0xFF);
        }

        return padded;
    }

    static void processChunks(const std::string& padded, std::vector<uint32_t>& hashBuffer) {
        size_t numChunks = padded.length() * 8 / 512;

        for (size_t i = 0; i < numChunks; ++i) {
            uint32_t w[80];
            for (size_t j = 0; j < 16; ++j) {
                w[j] = 0;
                for (size_t k = 0; k < 4; ++k) {
                    w[j] |= static_cast<uint8_t>(padded[i * 64 + j * 4 + k]) << (24 - k * 8);
                }
            }

            for (size_t j = 16; j < 80; ++j) {
                w[j] = leftRotate(w[j - 3] ^ w[j - 8] ^ w[j - 14] ^ w[j - 16], 1);
            }

            uint32_t a = hashBuffer[0];
            uint32_t b = hashBuffer[1];
            uint32_t c = hashBuffer[2];
            uint32_t d = hashBuffer[3];
            uint32_t e = hashBuffer[4];

            for (int j = 0; j < 80; ++j) {
                uint32_t f, k;
                if (j < 20) {
                    f = (b & c) | (~b & d);
                    k = 0x5A827999;
                }
                else if (j < 40) {
                    f = b ^ c ^ d;
                    k = 0x6ED9EBA1;
                }
                else if (j < 60) {
                    f = (b & c) | (b & d) | (c & d);
                    k = 0x8F1BBCDC;
                }
                else {
                    f = b ^ c ^ d;
                    k = 0xCA62C1D6;
                }

                uint32_t temp = leftRotate(a, 5) + f + e + k + w[j];
                e = d;
                d = c;
                c = leftRotate(b, 30);
                b = a;
                a = temp;
            }

            hashBuffer[0] += a;
            hashBuffer[1] += b;
            hashBuffer[2] += c;
            hashBuffer[3] += d;
            hashBuffer[4] += e;
        }
    }

    static uint32_t leftRotate(uint32_t value, size_t count) {
        return (value << count) | (value >> (32 - count));
    }
};
