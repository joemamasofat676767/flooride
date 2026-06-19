#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <vector>
#include <sstream>
#include <array>

template<typename type>
struct mat{
	type word;
	std::vector<type> next;
	std::vector<float> embedding;
};

// these fetch functions are for binary mode
unsigned char* FetchWord(std::vector<unsigned char>* binary){
	unsigned char* result = new unsigned char[binary->size()]();
	short i = 0;
	for(const auto& byte : *binary){
		result[i] = byte;
		i++;
	}
	return result;
}
unsigned char* FetchWord(std::vector<unsigned char>* binary){
	std::vector<unsigned char*>* result = new std::vector<unsigned char*>();
	std::vector<unsigned char> word;
	for(const auto& byte : *binary){
		if(byte != '\0'){
			word.push_back(byte);
		}
		else{
			result.push_back(FetchWord(&word));
			word.clear();
		}
	}
	return &result;
}

int main(){
	std::vector<mat*> mats;
	std::string path = "file.flrd";
	bool report = true;
	
	std::ifstream file_in(path);
	std::string contents;
	std::getline(file_in, contents);
	
	// 1 = binary, 0 = text
	bool mode;
	int dims;
	unsigned char JesusByte;
	if(contents.size() > 0 && file_in.is_open()){
		JesusByte = contents[0];
		mode = (JesusByte >> 7) & 1;
	 	dims= JesusByte & 0b01111111;
		if(!mode){
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
				mat* word_temp = new mat{word_name, next, embeddings}();
				mats.push_back(word_temp);
				contents += "\n" + line;
			}
		}
		else{
			file_in.seekg(1, std::ios::binary);
			/* a = fetching word, b = fetching next words, c = fetching embeddings, d = create mat */
			char state = 'a';
			std::vector<unsigned char> binary;
			unsigned char* byte;
			unsigned char* word;
			unsigned char* next;
			while(file_in.get(byte)){
				switch(state){
					case 'a':
						if(byte != '\x1f'){
							binary.push_back(byte);
						}
						else{
							word = FetchWord(&binary);
							binary.clear();
							state = 'b';
						}
						break;

					case 'b':
						if(byte != '\x1f'){
							binary.push_back(byte);
						}
						else{
							next = new FetchNext(&binary);
							binary.clear();
							state = 'c';
						}
						break;

					case 'd':
						delete[] word;
						delete[] next;
						break;
				}
				// switch(state){
				// 	case 'a':
				// 		word = new unsigned char[(short)byte]; 
				// 		state = 'b';
				// 		break;

				// 	case 'b':
				// 		if(i < sizeof(word)){
				// 			word += (char)byte;
				// 			i++;
				// 		}
				// 		else{
				// 			i = 0;
				// 			state = 'c';
				// 		}
				// 		break;

				// 	case 'c':
				// 		next_length = new unsigned char[(short)byte]; 
				// 		state = 'd';
				// 		break;

				// 	case 'd':
				// 		if(i < sizeof(next_length)){
				// 			next_length += (char)byte;
				// 			i++;
				// 		}
				// 		else{
				// 			i = 0;
				// 			state = 'e';
				// 		}
				// 		break;
					
				// 	case 'e':
						
				// 		break;

				// 	case 'f':
						
				// 		break;
				// }
			}
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