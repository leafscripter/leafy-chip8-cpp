#include <iostream>
#include <fstream>
#include <vector>
#include "includes/chip8.hpp"

using namespace chip8;

// First 512 bytes of CHIP8 memory are reserved
constexpr size_t kReservedBytes {512};
constexpr size_t kScreenWidth {32};
constexpr size_t kScreenHeight{64};

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

uint16_t Chip8::fetch_opcode() {
	uint16_t lower_opcode_half; 
	uint8_t upper_opcode_half;

	lower_opcode_half = memory.at(pc);
	lower_opcode_half <<= 8;
	upper_opcode_half = memory.at(pc++);

	pc += 2;

	return lower_opcode_half | upper_opcode_half;
}

Opcode Chip8::get_enum_format(uint16_t opcode) {
	switch (opcode) {
		case 0x0E00:
			return Opcode::kClearScreen;
		case 0x1000:
			return Opcode::kJump;
		case 0x6000:
			return Opcode::kSetVX;
		case 0x7000:
			return Opcode::kAddVX;
		case 0xA000:
			return Opcode::kSetIndexRegister;
		case 0xD000:
			return Opcode::kDraw;
		default:
			return Opcode::kUnsupported;
			break;
	}
	return Opcode::kUnsupported;
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

static constexpr uint16_t mask_x = 0x0f00;
static constexpr uint8_t mask_y = 0x00f0;
static constexpr uint8_t mask_n = 0x000f;
static constexpr uint8_t mask_nn = 0x00ff;
static constexpr uint16_t mask_nnn = 0x0fff;

inline uint16_t MaskBytes(uint16_t bytes, uint16_t mask, uint8_t bits_to_shift=0) {	return (bytes & mask) >> bits_to_shift; }

void Chip8::ExecuteClockCycle() {
	Opcode enum_opcode;

	uint16_t opcode = fetch_opcode();
	uint16_t address = MaskBytes(opcode, mask_nnn);

	uint8_t x = MaskBytes(opcode, mask_x, 8); // used to index registers VX-VF
	uint8_t y = MaskBytes(opcode, mask_y, 4); // used to index registers VY-VF
	uint8_t n = MaskBytes(opcode, mask_n); // a 4-bit number
	uint8_t nn = MaskBytes(opcode, mask_nn); // an 8-bit immediate number

	enum_opcode = get_enum_format(opcode);
	switch (enum_opcode) {
		case Opcode::kUnsupported:
			std::cout << "Error: CHIP8 instruction set does not support this opcode\n";
			return;
		case Opcode::kClearScreen:
			break;
		case Opcode::kJump:
			pc = address;
			break;
		case Opcode::kSetVX:
			v[x] = nn;
			break;
		case Opcode::kAddVX:
			v[x] += nn;
			break;
		case Opcode::kSetIndexRegister:
			I = address;
			break;
		case Opcode::kDraw: 
			uint8_t x_pos = v[x] & 63;
			uint8_t y_pos = v[y] & 31;
	}
}

