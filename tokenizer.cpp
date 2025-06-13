#include "tokenizer.h"


namespace Service {
std::string TokenGen::GenerateHEXToken()
{
  uint64_t first = generator1_();
  uint64_t second = generator2_();
  return NumToHex(first) + NumToHex(second);
}

std::string TokenGen::NumToHex(uint64_t num)
{
  std::stringstream ss;
  ss << std::hex << num;
  std::stringstream ss2;
  ss2 << std::setw(16) << std::setfill('0') << ss.str();
  return ss2.str();
}
TokenGen Token::GENERATOR;

}