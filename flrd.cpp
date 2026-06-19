#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <vector>
#include <sstream>
#include <array>
#include <variant>

template<typename type>
struct mat{
	type word;
	std::vector<type> next;
	std::vector<float> embeddings;
	mat(type w, std::vector<type> n, std::vector<float> e){
		word = w;
		next = n;
		embeddings = e;
	}
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
std::vector<unsigned char*>* FetchNext(std::vector<unsigned char>* binary){
	std::vector<unsigned char*> result;
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
std::vector<unsigned char*>* FetchEmbeddings(std::vector<unsigned char>* binary){
	std::vector<unsigned char*> result;
	std::vector<unsigned char> embedding;
	for(const auto& byte : *binary){
		if(byte != ','){
			embedding.push_back(byte);
		}
		else{
			result.push_back(FetchWord(&embedding));
			embedding.clear();
		}
	}
	return &result;
}

int main(){
	auto* mats = new std::variant<std::vector<mat<char[]>*>*, std::vector<mat<std::string>*>*>();
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
		if(mode){
			mats = new std::vector<mat<char[]>*>();
		}
		else{
			mats = new std::vector<mat<std::string>*>();
		} 
	 	dims = JesusByte & 0b01111111;
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
				mat<std::string>* MatObj_temp = new (mat<std::string>(word_name, next, embeddings))();
				*mats.push_back(MatObj_temp);
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
			unsigned char* embeddings;
			while(file_in.get(byte)){
				switch(state){
					case 'a':
						if(*byte != 0x1F){
							binary.push_back(byte);
						}
						else{
							word = FetchWord(&binary);
							binary.clear();
							state = 'b';
						}
						break;

					case 'b':
						if(*byte != 0x1F){
							binary.push_back(byte);
						}
						else{
							next = FetchNext(&binary);
							binary.clear();
							state = 'c';
						}
						break;

					case 'c':
						if(*byte != 0x1F){
							binary.push_back(byte);
						}
						else{
							embeddings = FetchEmbeddings(&binary);
							binary.clear();
							state = 'd';
						}
						break;

					case 'd':
						mat<char[]>* MatObj_temp = new (mat<char[]>(word, next, embeddings))();
						*mats.push_back(MatObj_temp);
						state = 'a';
						delete[] word;
						delete next;
						delete embeddings;
						break;
				}
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
	for(const auto& thing : *mats){
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
	delete mats;
	return 0;
}