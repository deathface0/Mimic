#include <iostream>
#include "MimicEngine.h"
#include "Hook.h"

int main()
{
	Hook hook;
	MimicEngine m;

	Sleep(1000);
	std::cout << "RECORDING..." << std::endl;
	m.record_start();
	Sleep(5000);
	m.record_stop();
	std::cout << "RECORDING END" << std::endl;

	std::cout << "export: " << m.exportBuffer("C:\\Users\\Deathface\\Desktop\\exportcmd.json") << std::endl;
	Sleep(1000);


	m.readFile("C:\\Users\\Deathface\\Desktop\\exportcmd.json");

	auto start = std::chrono::high_resolution_clock::now();
	m.run();
	auto end = std::chrono::high_resolution_clock::now();
	auto elap = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	std::cout << "TOTAL MACRO: " << elap.count() << "ms" << std::endl;

	return 0;
}