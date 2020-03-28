#include "bidirectional_dictionary.hpp"
#include <iostream>


int main(int argc, char* argv[])
{
	bidirectinal_dictionary<std::string, std::string> dict;
	dict.emplace("firstApple", "secondApple");
	dict.emplace("firstBanana", "secondBanana");
	dict.emplace("firstWatermelon", "secondWatermelon");
	dict.emplace("firstMelon", "secondMelon");

	std::cout << dict.first("secondApple") << " <=> " << dict.second("firstApple") << std::endl;
	std::cout << dict.first("secondBanana") << " <=> " << dict.second("firstBanana") << std::endl;
	std::cout << dict.first("secondWatermelon") << " <=> " << dict.second("firstWatermelon") << std::endl;
	std::cout << dict.first("secondMelon") << " <=> " << dict.second("firstMelon") << std::endl;

	return 0;
}