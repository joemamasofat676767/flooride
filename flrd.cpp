#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <cstdint>

int main(){
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
			contents += "\n" + line;
		}
	}
	else{
		JesusByte = 0b011;
		if(report){
			std::cerr << "file was not found/empty. created/added missing data.\n";
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