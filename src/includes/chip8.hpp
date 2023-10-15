#pragma once 

#include <array>
#include <stack>
#include <string>

namespace chip8 {
	enum class Chip8Instruction {
		CLEAR, // Clear the screen
		JUMP, // Jump to address nnn

		SET_VX_VY, // VX is set to the value of VY
		SET_VX_NN, // Set VX to a byte
		ADD_VX_NN, // Add byte to VX
		ADD_VX_VY, // Add value of VY to VX, sets carry flag if result > 255
		SUB_VX_VY, // Store result of VX - VY in VX
		SUB_VY_VX, // Store result of VY - VX in VX

		SHIFTR, // shift to the right
		SHIFTL, // shift to the left


		I_NNN, // Set Index
		DRAW, // Self-explanatory

		BINARY_OR, // VX is set to the result of (VX OR VY)
		BINARY_AND, // VX is set to the result of (VX AND VY)
		LOGICAL_XOR, // VX is set to the result of (VX XOR VY)

		UNSUPPORTED, 
	};

	class Chip8 {
		public:
			bool draw_flag;
			bool clear_flag;
			Chip8(std::array<uint8_t, 80> fontset);
			void emulate_cycle();
			bool load(std::string const &path);

		private:
			std::stack<uint16_t> stack;
			std::array<uint8_t, 16> v; // general purpose register
			std::array<uint8_t, 4096> memory;
			std::array<std::array<uint8_t, 32>, 64> graphics; 
			uint8_t delay_timer;
			uint8_t sound_timer;
			uint16_t I;
			uint16_t pc;
			uint16_t current_opcode;

			uint16_t Chip8::fetch_opcode();
			Chip8Instruction decode(uint16_t opcode);
	};
}
