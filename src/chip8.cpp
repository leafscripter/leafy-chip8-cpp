#include <iostream>
#include <fstream>
#include <vector>
#include "includes/chip8.hpp"

using namespace chip8;

// First 512 bytes of CHIP8 memory are reserved
static constexpr size_t kReservedBytes {512};
static constexpr size_t kScreenWidth {32};
static constexpr size_t kScreenHeight{64};

// Defining masks for the decoding process
static constexpr uint16_t mask_x = 0x0f00;
static constexpr uint8_t mask_y = 0x00f0;
static constexpr uint8_t mask_n = 0x000f;
static constexpr uint8_t mask_nn = 0x00ff;
static constexpr uint16_t mask_nnn = 0x0fff;

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

void Chip8::set_draw_flag(bool state) {
	draw_flag = state;
}

bool Chip8::get_draw_flag() {
	return draw_flag;
}

bool Chip8::get_clear_flag() {
	return clear_flag;
}

void Chip8::set_clear_flag(bool state) {
	clear_flag = state;
}

Opcode Chip8::fetch_opcode() {
	Opcode op{};
	uint16_t opcode{};
	
	/*Fetch two bytes in succession
	Merge both bytes into one 16-bit instruction 
	Convert it to an enum class*/
	opcode = memory[pc];
	opcode <<= 8;
	opcode |= memory[pc + 1]; 
	op = get_enum_format(opcode);

	current_opcode = opcode; // storing for later use

	pc += 2; // get ready to fetch next instruction

	return op;
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
	}
	return Opcode::kUnsupported;
}

bool Chip8::load_rom(std::string const &path) {
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


inline uint16_t ExtractNibbles(uint16_t bytes, uint16_t mask, uint8_t bits_to_shift=0) {	
	return (bytes & mask) >> bits_to_shift; 
}

void Chip8::emulate_cycle() {

	// Fetching stage
	Opcode op = fetch_opcode();

	// Decoding stage
	uint16_t address = ExtractNibbles(current_opcode, mask_nnn);
	uint8_t x = ExtractNibbles(current_opcode, mask_x, 8); // used to index registers VX-VF
	uint8_t y = ExtractNibbles(current_opcode, mask_y, 4); // used to index registers VY-VF
	uint8_t n = ExtractNibbles(current_opcode, mask_n); // a 4-bit number
	uint8_t nn = ExtractNibbles(current_opcode, mask_nn); // an 8-bit immediate number

	// Execution stage
	switch (op) {
		case Opcode::kUnsupported:
			std::cout << "Error: CHIP8 instruction set does not support this opcode\n";
			return;
		case Opcode::kClearScreen:
			clear_flag = true;
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
			draw_flag = true;
			break;
	}
}

