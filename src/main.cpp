#include <iostream>
#include "includes/chip8.hpp"
#include <SFML/Graphics.hpp>
#include <array>

using chip8::Chip8;

std::array<uint8_t, 80> fontset = {
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

void RenderGraphics(Chip8 &ch8, sf::RenderWindow* window);

int main() {

	// Initialize the emulator with the fontset
	Chip8 emulator(fontset);

	std::string path;
	std::cout << "Enter valid name: ";
	std::cin >> path;
	
	emulator.load(path);

	sf::RenderWindow window(sf::VideoMode(64, 32), "Chip8");

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// run a CHIP8 CPU cycle
		emulator.emulate_cycle();

		// clear screen if the instruction says so
		if (emulator.clear_flag) {
			window.clear(sf::Color::Black);
			!emulator.clear_flag;
		}

		// draw if the flag is set
		if (emulator.draw_flag) {
			RenderGraphics(&emulator);
			!emulator.draw_flag;
		}
	}

	return 0;
}

void RenderGraphics(Chip8 &ch8, sf::RenderWindow* window) {
	window.clear(); 

	// Implement drawing logic here

	window.display();
}
