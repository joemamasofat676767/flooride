#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <vector>
#include <sstream>
#include <array>
#include <variant>
#include <cstring>
#include <type_traits>

template<typename type>
struct mat{
	type word;
	std::vector<type> next;
	std::vector<float> embeddings;

	mat(type w, std::vector<type> n, std::vector<float> e){
		if constexpr (std::is_same_v<type, unsigned char*>){
			word = new unsigned char[sizeof(w)]();
			std::memcpy(word, w, sizeof(w));
		}
		else{
			word = w;
		}
		next = n;
		embeddings = e;
	}
};

// these fetch functions are for binary mode
std::vector<unsigned char> FetchWord(std::vector<unsigned char>* binary){
	std::vector<unsigned char> result;
	for(const auto& byte : *binary){
		result.push_back(byte);
	}
	return result;
}
std::vector<std::vector<unsigned char>> FetchNext(std::vector<unsigned char>* binary){
	std::vector<std::vector<unsigned char>> result;
	std::vector<unsigned char> word;
	for(const auto& byte : *binary){
		if(byte != 0){
			word.push_back(byte);
		}
		else{
			result.push_back(FetchWord(&word));
			word.clear();
		}
	}
	return result;
}
std::vector<float> FetchEmbeddings(std::vector<unsigned char>* binary){
	short chunk_i = 0;
	std::vector<float> result;
	std::vector<unsigned char> embedding_raw;
	float embedding;
	for(const auto& byte : *binary){
		if(chunk_i != 4){
			embedding_raw.push_back(byte);
		}
		else{
			std::vector<unsigned char> embedding_temp = FetchWord(&embedding_raw);
			std::memcpy(&embedding, &embedding_temp, sizeof(float));
			result.push_back(embedding);
			embedding_raw.clear();
		}
	}
	return result;
}

int main(){
	std::string path = "file.flrd";
	bool report = true;
	
	std::ifstream file_in(path, std::ios::binary);
	std::string contents;
	
	// 1 = binary, 0 = text
	bool mode;
	int dims;
	unsigned char JesusByte;
	file_in.read(reinterpret_cast<char*>(&JesusByte), 1);
	std::vector<mat<std::vector<unsigned char>>*> mats_bin;
	std::vector<mat<std::string>*> mats_txt;
	if(file_in.is_open()){
		mode = (JesusByte >> 7) & 1;
		dims = JesusByte & 0b01111111;
		std::cout << "Jesus byte: 0x" << std::hex << (int)JesusByte << std::dec << std::endl;
		std::cout << "Mode: " << mode << std::endl;
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
				mat<std::string>* MatObj_temp = new mat<std::string>(word_name, next, embeddings);
				mats_txt.push_back(MatObj_temp);
				contents += "\n" + line;
			}
		}
		else{
			file_in.seekg(1);
			/* a = fetching word, b = fetching next words, c = fetching embeddings, d = create mat */
			char state = 'a';
			std::vector<unsigned char> binary;
			unsigned char byte;
			std::vector<unsigned char> word;
			std::vector<std::vector<unsigned char>> next;
			std::vector<float> embeddings;
			unsigned char[sizeof(float)] chunk;
			int ch;
			while((ch = file_in.get()) != EOF){
				byte = static_cast<unsigned char>(ch);
				switch(state){
					case 'a':
						std::cout << "'a': " << (int)byte << "\n";
						if(byte != 0x1F){
							binary.push_back(byte);
						}
						else{
							word = FetchWord(&binary);
							binary.clear();
							state = 'b';
						}
						std::cout << "reached state a\n";
						break;

					case 'b':
						std::cout << "'b': " << (int)byte << "\n";
						if(byte != 0x1F){
							binary.push_back(byte);
						}
						else{
							next = FetchNext(&binary);
							binary.clear();
							state = 'c';
						}
						std::cout << "reached state b\n";
						break;

					case 'c':
						std::cout << "'c': " << (int)byte << "\n";
						if(byte != 0x1F){
							binary.push_back(byte);
						}
						else{
							for(const auto& embedding: FetchEmbeddings(&binary)){
								embeddings.push_back(embedding);
							}
							binary.clear();
							state = 'd';
						}
						std::cout << "reached state c\n";
						break;

					case 'd':
						mat<std::vector<unsigned char>>* MatObj_temp = new mat<std::vector<unsigned char>>(word, next, embeddings);
						mats_bin.push_back(MatObj_temp);
						std::cout << "reached state d\n";
						state = 'a';
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
	for(const auto& thing : mats_bin){
		std::cout << thing << ":";
		std::cout << reinterpret_cast<const char*>((thing->word).data()) << "\n";
		for(const auto& next : thing->next){
			for(const auto& ch : next){
				std::cout << ch;
			}
			std::cout << ",";
		}
		for(const auto& embedding : thing->embeddings){
			std::cout << embedding << ",";
		}
		std::cout << "\n";
	}
	
	if(mode){
		std::cout << "binary mode";
	}
	else{
		std::cout << "text mode";
	}

	if(mats_bin.empty()){
		std::cout << "nothing";
	}

	if(mode){
		std::ofstream file_out(path, std::ios::binary);
		file_out.write(reinterpret_cast<const char*>(&JesusByte), 1);
		for(const auto& data : mats_bin){
			file_out.write(reinterpret_cast<const char*>((data->word).data()), sizeof(data->word));

			short NextItemSize = sizeof(data->next);
			for(const auto& NextWord : data->next){
				file_out.write(reinterpret_cast<const char*>(NextWord.data()), sizeof(NextWord));
				if(NextItemSize != 0){
					file_out.put(0);
				}
				NextItemSize--;
			}
			file_out.put(0x1F);

			short i = 0;
			for(const auto& embedding : data->embeddings){
				file_out.write(reinterpret_cast<const char*>(&embedding), sizeof(float));
				if(i != dims){
					file_out.put(0);
				}
				i++;
			}
			file_out.put(0x1F);
		}
		file_out.flush();
		file_out.close();
	}
	else{
		std::ofstream file_out(path);
		file_out.write(reinterpret_cast<char*>(&JesusByte), 1);
		file_out.write(contents.data(), contents.size());
		file_out.flush();
		file_out.close();
	}

	return 0;
}