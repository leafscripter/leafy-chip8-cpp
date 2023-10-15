#pragma once 

#include <array>
#include <stack>
#include <string>

namespace chip8 {

	enum class Opcode {
		kClearScreen,
		kJump,
		kSetVX,
		kAddVX,
		kSetIndexRegister,
		kDraw,
		kUnsupported
	};

	class Chip8 {
		public:
			Chip8(std::array<uint8_t, 80> fontset);
			void emulate_cycle();
			bool load_rom(std::string const &path);
			void set_draw_flag(bool state);
			void set_clear_flag(bool state);
			bool get_draw_flag();
			bool get_clear_flag();

		private:
			std::stack<uint16_t> stack;
			std::array<uint8_t, 16> v; // general purpose register
			std::array<uint8_t, 4096> memory;
			std::array<std::array<uint8_t, 32>, 64> graphics;
			bool draw_flag;
			bool clear_flag; 
			uint8_t delay_timer;
			uint8_t sound_timer;
			uint16_t I;
			uint16_t pc;
			uint16_t current_opcode;

			Opcode get_enum_format(uint16_t opcode);
			Opcode fetch_opcode();
	};
}
