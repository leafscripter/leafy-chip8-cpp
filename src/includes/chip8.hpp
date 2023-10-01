#pragma once 

#include <array>
#include <stack>
#include <string>

namespace chip8 {
	class Chip8 {
		public:
			Chip8(std::array<uint8_t, 80> fontset);
			void ExecuteClockCycle();
			bool load_rom_into_memory(std::string const &path);

		private:
			std::stack<uint16_t> stack;
			std::array<uint8_t, 16> v; // general purpose register
			std::array<uint8_t, 4096> memory;
			std::array<std::array<uint8_t, 32>, 64> graphics;
			bool draw_flag; 
			uint8_t delay_timer;
			uint8_t sound_timer;
			uint16_t I;
			uint16_t pc;
	};
}
