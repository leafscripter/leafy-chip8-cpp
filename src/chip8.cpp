#include <iostream>
#include <fstream>
#include <vector>
#include "includes/chip8.hpp"

using namespace chip8;

// For added readability
typedef uint16_t WORD;
typedef uint8_t BYTE;

// First 512 bytes of CHIP8 memory are reserved
static constexpr size_t kReservedBytes {512};
static constexpr size_t kScreenWidth {32};
static constexpr size_t kScreenHeight{64};

// Defining bitmasks for the decoding process
static constexpr WORD bitmask_x {0x0f00};
static constexpr WORD bitmask_nnn {0x0fff};
static constexpr BYTE bitmask_y {0x00f0};
static constexpr BYTE bitmask_n {0x000f};
static constexpr BYTE bitmask_nn = {0x00ff};
static constexpr BYTE bitmask_id = {0xf000};

inline WORD ApplyBitmaskToWord(WORD word, WORD bitmask, BYTE lshift=0) {	
	word &= bitmask; 
	
	if (lshift)
		word >>= lshift;

	return word; 
}

Chip8::Chip8(std::array<uint8_t, 80> fontset): 
	stack{}, // holds 8 16-bit addresses
	v{}, // general purpose registers from V0-VF
	memory{}, // chip8 memory, first 512 bytes reserved
	graphics{},
	draw_flag{false},
	delay_timer{0},
	sound_timer{0},
	I{0},
	pc{kReservedBytes}, // program counter must start at 512 to execute rom
	current_opcode{}
{
	std::copy(fontset.begin(), fontset.end(), memory.begin());
}

WORD Chip8::fetch_opcode() {
	WORD opcode{};

	opcode = memory[pc]; // fetch first byte
	opcode <<= 8;  // shift 8 bits to the right to make space for the second byte
	opcode |= memory[pc + 1]; // fetch the second byte, and then merge with first byte

	return opcode;
}

inline Chip8Instruction DecodeLogicalInstruction(BYTE n) {
	switch (n) {
		case 0:
			return Chip8Instruction::SET_VX_VY;
		case 1:
			return Chip8Instruction::BINARY_OR;
		case 2:
			return Chip8Instruction::BINARY_AND;
		case 3:
			return Chip8Instruction::LOGICAL_XOR;
		case 4:
			return Chip8Instruction::ADD_VX_VY;
		case 5:
			return Chip8Instruction::SUB_VX_VY;
		case 6:
			return Chip8Instruction::SHIFTR;
		case 7:
			return Chip8Instruction::SUB_VY_VX;
		case 14:
			return Chip8Instruction::SHIFTL;
	}
}

Chip8Instruction Chip8::decode(WORD opcode) {
	BYTE id = ApplyBitmaskToWord(opcode, bitmask_id, 12);
	BYTE n = ApplyBitmaskToWord(opcode, bitmask_n);
	switch (id) {
		case 0:
			return Chip8Instruction::CLEAR;
		case 1:
			return Chip8Instruction::JUMP;
		case 6:
			return Chip8Instruction::SET_VX_NN;
		case 7:
			return Chip8Instruction::ADD_VX_NN;
		case 10: // 0xA000
			return Chip8Instruction::I_NNN;
		case 13: // 0xD000
			return Chip8Instruction::DRAW;
		case 8:
			DecodeLogicalInstruction(n);
	}
	return Chip8Instruction::UNSUPPORTED;
}

bool Chip8::load(std::string const &path) {
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

void Chip8::emulate_cycle() {

	// Fetching stage
	WORD opcode = fetch_opcode();
	pc += 2;
	
	// Decoding stage
	WORD address = ApplyBitmaskToWord(opcode, bitmask_nnn);
	BYTE x = ApplyBitmaskToWord(opcode, bitmask_x, 8); // used to index registers VX-VF
	BYTE y = ApplyBitmaskToWord(opcode, bitmask_y, 4); // used to index registers VY-VF
	BYTE n = ApplyBitmaskToWord(opcode, bitmask_n); // a 4-bit number
	BYTE nn = ApplyBitmaskToWord(opcode, bitmask_nn); // an 8-bit immediate number

	Chip8Instruction instruction = decode(opcode);

	// Execution stage
	switch (instruction) {
		case Chip8Instruction::UNSUPPORTED:
			std::cout << "Error: CHIP8 instruction set does not support this opcode\n";
			return;
		case Chip8Instruction::CLEAR:
			clear_flag = true;
			break;
		case Chip8Instruction::JUMP:
			pc = address;
			break;
		case Chip8Instruction::SET_VX_NN:
			v[x] = nn;
			break;
		case Chip8Instruction::ADD_VX_NN:
			v[x] += nn;
			break;
		case Chip8Instruction::I_NNN:
			I = address;
			break;
		case Chip8Instruction::DRAW: 
			draw_flag = true;
			break;
	}
}

