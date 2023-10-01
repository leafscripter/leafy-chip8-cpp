#include <iostream>
#include <fstream>
#include <vector>
#include "includes/chip8.hpp"

using namespace chip8;

// First 512 bytes of CHIP8 memory are reserved
constexpr size_t kReservedBytes {512};
constexpr size_t kScreenWidth {32};
constexpr size_t kScreenHeight{64};

// Aliases for CHIP8 instructions
constexpr uint16_t kOpcodeJmpToAddr {0x1000};
constexpr uint16_t kOpcodeSet {0x6000};
constexpr uint16_t kOpcodeAdd {0x7000};
constexpr uint16_t kOpcodeSetIndex {0xA000};
constexpr uint16_t kOpcodeDraw {0xD000};

Chip8::Chip8(std::array<uint8_t, 80> fontset): 
	stack{}, // holds 8 16-bit addresses
	v{}, // general purpose registers from V0-VF
	memory{}, // chip8 memory, first 512 bytes reserved
	graphics{},
	draw_flag{false},
	delay_timer{0},
	sound_timer{0},
	I{0},
	pc{512} // program counter must start at 512 to execute rom
{
	std::copy(fontset.begin(), fontset.end(), memory.begin());
}


bool Chip8::load_rom_into_memory(std::string const &path) {
	std::ifstream file(path, std::ios::binary | std::ios::ate);

	if (!file) // failed to open file!
		return false;

	std::ifstream::pos_type file_size = file.tellg();

	if (file_size == std::ifstream::pos_type(-1)) // tell failed!
		return false;

	file.seekg(0, std::ios::beg);

	if (!file) // seek failed!
		return false;

	if (file_size > memory.size() - kReservedBytes) // rom won't fit
		return false;

	std::vector<uint8_t> buffer(file_size);
	file.read(reinterpret_cast<char*>(buffer.data()), file_size);

	if (!file) // read failed!
		return false;

	std::copy(buffer.begin(), buffer.end(), memory.begin() + kReservedBytes);
	return true;
}

void Chip8::ExecuteClockCycle() {

	uint16_t opcode{};
	uint8_t x{}, y{}, n{}, nn{};
	uint16_t address{};

	opcode = memory.at(pc);
	opcode <<= 8;
	opcode |= memory.at(++pc);

	x = (opcode & 0x0f00) >> 8; // used to index registers VX-VF
	y = (opcode & 0x00f0) >> 4; // used to index registers VY-VF
	n = opcode & 0x000f; // a 4-bit number
	nn = opcode & 0x00ff; // an 8-bit immediate number
	address = opcode & 0x0fff; // a 12-bit immediate address

	switch (opcode) {
		case kOpcodeJmpToAddr:
			pc = address;
			break;
		case kOpcodeSet:
			v[x] = nn;
			break;
		case kOpcodeAdd:
			v[x] += nn;
			break;
		case kOpcodeSetIndex:
			I = address;
			break;
		case kOpcodeDraw: 
			uint8_t x_pos = v[x] & 63;
			uint8_t y_pos = v[y] & 31;
	}
}

