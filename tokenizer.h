#pragma once
#include <random>
#include <sstream>
#include <iomanip>

namespace Service {

class TokenGen
{
public:
    std::string GenerateHEXToken();
private:
   
    std::string NumToHex(uint64_t num);

    std::random_device random_device_;
    std::mt19937_64 generator1_{[this]
                                {
                                    std::uniform_int_distribution<std::mt19937_64::result_type> dist;
                                    return dist(random_device_);
                                }()};
    std::mt19937_64 generator2_{[this]
                                {
                                    std::uniform_int_distribution<std::mt19937_64::result_type> dist;
                                    return dist(random_device_);
                                }()};
};

class Token
{
public:
    static TokenGen GENERATOR;
};

}
