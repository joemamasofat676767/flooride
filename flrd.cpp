#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include <sstream>

struct MatStruct{
	std::vector<std::string> next;
	std::array<float, 3> embedding;
};

using mat = std::unordered_map<std::string, MatStruct>;

int main(){
	std::vector<int*> mats;
	std::string path = "file.flrd";
	bool report = true;
	
	std::ifstream file_in(path);
	std::string contents;
	std::getline(file_in, contents);
	
	unsigned char JesusByte;
	if(contents.size() > 0 && file_in.is_open()){
		JesusByte = contents[0];
		contents = contents.substr(1);
		std::string line;
		while(std::getline(file_in, line)){
			std::stringstream line_stream(line);

			mat* word_temp = {line.substr(0, line.find('[')), };
			mats.push_back();
			contents += "\n" + line;
		}
	}
	else{
		JesusByte = 0b011;
		if(report){
			std::cerr << "file absent: overhauled\n";
		}
	}
	file_in.close();
	std::cout << std::bitset<8>(JesusByte) << "\n";
	//1 = binary, 0 = text
	bool mode = (JesusByte >> 7) & 1;
	std::int8_t dims= JesusByte & 0b01111111;
	std::cout << mode << " | " << (int)dims << "\n";
	std::cout << contents << "\n";
	
	if(mode){
		std::cout << "binary mode";
	}
	else{
		std::cout << "text mode";
	}

	std::ofstream file_out(path, std::ios::binary);
	file_out.write(reinterpret_cast<char*>(&JesusByte), 1);
	file_out << contents;
	file_out.flush();
	file_out.close();
	return 0;
}