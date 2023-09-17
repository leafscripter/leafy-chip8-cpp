#include <iostream>
#include <fstream>
#include <vector>
#include "includes/emulator.hpp"

using namespace emulator;

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

Emulator::Emulator(): 
	stack{}, // holds 8 16-bit addresses
	gp_register{}, // general purpose registers from V0-VF
	memory{}, // chip8 memory, first 512 bytes reserved
	graphics{},
	draw_flag{false},
	delay_timer{0},
	sound_timer{0},
	I{0},
	pc{512} // program counter must start at 512 to execute rom
{
	std::array<uint8_t, 80> fontset {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};
	std::copy(fontset.begin(), fontset.end(), memory.begin());
}

inline uint16_t Emulator::fetch_opcode() {
	uint16_t lower_opcode_half = memory.at(pc);  
	uint8_t higher_opcode_half = memory.at(++pc); 

	return (lower_opcode_half << 8) | higher_opcode_half;
}

bool Emulator::load_rom_into_memory(std::string const &path) {
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

void Emulator::PerformCpuCycle() {

	uint16_t opcode {fetch_opcode()};

	uint8_t x = (opcode & 0x0f00) >> 8; 
	uint8_t y = (opcode & 0x00f0) >> 4;
	uint8_t n = opcode & 0x000f;
	uint8_t nn = opcode & 0x00ff;
	uint16_t addr = opcode & 0x0fff;

	switch (opcode) {
		case kOpcodeJmpToAddr:
			pc = addr;
			break;
		case kOpcodeSet:
			gp_register[x] = nn;
			break;
		case kOpcodeAdd:
			gp_register[x] += nn;
			break;
		case kOpcodeSetIndex:
			I = addr;
			break;
	}
}

