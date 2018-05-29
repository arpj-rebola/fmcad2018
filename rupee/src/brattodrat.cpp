#include <fstream>
#include <iostream>
#include <string>
#include <limits>

constexpr std::int64_t RunShift = 7;
constexpr std::uint8_t RunMask = 0b1 << 7;
constexpr std::int64_t MaxShift = 63;

std::ifstream input;
std::ofstream output;
char read;
bool inclause;
std::uint8_t uread;
std::uint64_t unumber;
std::int64_t number;
std::int64_t shift;

int main(int argc, char* argv[]) {
	input.open(argv[1], std::ifstream::in | std::ifstream::binary);
	if(!input.good()) {
		std::cout << "e Could not open file " << argv[1] << " ." << std::endl;
		std::cout << "s FAIL" << std::endl;
		return 1;
	}
	output.open(argv[2], std::ofstream::out | std::ofstream::trunc);
	if(!output.good()) {
		std::cout << "e Could not open file " << argv[2] << " ." << std::endl;
		std::cout << "s FAIL" << std::endl;
		input.close();
		return 1;
	}
	inclause = false;
	while(true) {
		if(!inclause) {
			input.get(read);
			if(input.eof()) {
				input.close();
				output.close();
				std::cout << "s OK" << std::endl;
				return 0;
			} else if(read == 'd') {
				output << "d ";
			} else if(read == 'a') {
			} else {
				std::cout << "e Input error: could not parse file " << argv[1] << " ." << std::endl;
				std::cout << "s FAIL" << std::endl;
				input.close();
				output.close();
				return 1;
			}
			inclause = true;
		} else {
			unumber = 0;
			shift = 0;
			do {
				if(shift >= MaxShift) {
					std::cout << "e Overflow error: overflow while reading a variable-byte encoded number." << std::endl;
					std::cout << "s FAIL" << std::endl;
					input.close();
					output.close();
					return 1;
				}
				input.get(read);
				if(input.eof()) {
					std::cout << "e Input error: could not parse file " << argv[1] << " ." << std::endl;
					std::cout << "s FAIL" << std::endl;
					input.close();
					output.close();
					return 1;
				}
				uread = static_cast<std::uint8_t>(read);
				unumber |= (static_cast<std::uint64_t>(uread & ~RunMask) << shift);
				shift += RunShift;
			} while((uread & RunMask) != 0);
			number = ((unumber & 1) == 0) ? static_cast<std::int64_t>(unumber >> 1) : -(static_cast<std::int64_t>((unumber - 1) >> 1));
			output << number;
			if(number == 0) {
				output << '\n';
				inclause = false;
			} else {
				output << ' ';
			}
		}
	}
}