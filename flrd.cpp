#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <vector>
#include <sstream>
#include <array>

struct mat{
	std::string word;
	std::vector<std::string> next;
	std::vector<float> embedding;
};

int main(){
	std::vector<mat*> mats;
	std::string path = "file.flrd";
	bool report = true;
	
	std::ifstream file_in(path);
	std::string contents;
	std::getline(file_in, contents);
	
	unsigned char JesusByte;
	//1 = binary, 0 = text
	bool mode;
	int dims;
	if(contents.size() > 0 && file_in.is_open()){
		JesusByte = contents[0];
		mode = (JesusByte >> 7) & 1;
	 	dims= JesusByte & 0b01111111;
		contents = contents.substr(1);
		std::string line;
		while(std::getline(file_in, line)){
			std::string line_embedding = line.substr(line.find(']') + 1);
			line_embedding = line_embedding.substr(1, line_embedding.length() - 1);
			std::stringstream line_embedding_stream(line_embedding);
			std::stringstream line_stream(line);
			std::vector<std::string> next;
			std::string word;
			while(std::getline(line_stream, word, ',')){
				next.push_back(word);
			}
			std::vector<float> embeddings;
			int i = 0;
			std::string embedding;
			while(std::getline(line_embedding_stream, embedding, ',')){
				if(i >= dims){
					break;
				}
				embeddings.push_back(std::stof(embedding));
				i++;
			}
			std::string word_name = line.substr(0, line.find('['));
			mat* word_temp = new mat{word_name, next, embeddings};
			mats.push_back(word_temp);
			contents += "\n" + line;
		}
	}
	else{
		JesusByte = 0b011;
		mode = false;
		dims = 3;
		if(report){
			std::cerr << "file deficient: overhauled\n";
		}
	}
	file_in.close();
	std::cout << std::bitset<8>(JesusByte) << "\n";
	std::cout << mode << " | " << (int)dims << "\n";
	std::cout << contents << "\n";
	for(const auto& thing : mats){
		std::cout << thing << ":";
		std::cout << thing->word << ' ';
		for(const auto& next : thing->next){
			std::cout << next << ' ';
		}
		for(const auto& embedding : thing->embedding){
			std::cout << embedding  << ",";
		}
		std::cout << "\n";
	}
	
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