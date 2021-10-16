#include <chromapch.h>
#include <Chroma/Images/Aseprite.h>
#include <chrono>
#include <thread>

using namespace Chroma;

int main(int argc, char* argv[])
{
	Aseprite a = Aseprite(R"(C:\Users\Thomas\source\repos\Aesix\Assets\Textures\FOREST_TEMPLE_PROTO.aseprite)");
	std::this_thread::sleep_for(std::chrono::milliseconds(5000));
	return 0;
}