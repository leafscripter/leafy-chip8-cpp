#pragma once 

#include <array>
#include <stack>
#include <string>

namespace emulator {
	class Emulator {
		public:
			Emulator();
			void PerformCpuCycle();
			bool load_rom_into_memory(std::string const &path);

		private:
			std::stack<uint16_t> stack;
			std::array<uint8_t, 16> gp_register; // general purpose register
			std::array<uint8_t, 4096> memory;
			std::array<std::array<uint8_t, 32>, 64> graphics;
			bool draw_flag; 
			uint8_t delay_timer;
			uint8_t sound_timer;
			uint16_t I;
			uint16_t pc;
			

			unsigned short fetch_opcode();
			void execute_opcode(uint16_t *opcode);
	};
}
