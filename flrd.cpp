#include <iostream>
#include <fstream>
#include <string>
#include <bitset>
#include <vector>
#include <sstream>
#include <array>
#include <variant>
#include <cstring>
#include <tuple>
#include <algorithm>
#include <variant>

std::vector<unsigned char> FetchWord(std::vector<unsigned char>* binary);
std::vector<std::vector<unsigned char>> FetchNext(std::vector<unsigned char>* binary);
std::vector<float> FetchEmbeddings(std::vector<unsigned char>* binary);

typedef std::vector<unsigned char> bin;
typedef std::variant<std::string, bin> mat_WordBoth;
typedef std::variant<std::vector<std::string>, std::vector<bin>> mat_NextBoth;

class mat{
	private:
	mat_WordBoth word;
	mat_NextBoth next;
	std::vector<float> embeddings;

	public:
	
	inline static short dims;
	inline static unsigned char JesusByte;
	inline static bool mode;
	inline static bool report;
	inline static std::vector<mat*> mats;
	inline static std::string contents;
	
	static void lay(const std::string& path){	
		std::ifstream file_in(path, std::ios::binary);
		
		// 1 = binary, 0 = text
		file_in.read(reinterpret_cast<char*>(&JesusByte), 1);
		if(file_in.is_open()){
			mode = (JesusByte >> 7) & 1;
			dims = JesusByte & 0b01111111;
			std::cout << "Jesus byte: 0x" << std::hex << (int)JesusByte << std::dec << std::endl;
			std::cout << "Mode: " << mode << std::endl;
			file_in.seekg(1);
			if(!mode){
				std::string line;
				while(std::getline(file_in, line)){
					size_t bracket1 = line.find('[');
						size_t bracket2 = line.find(']', bracket1);
						size_t bracket3 = line.find('[', bracket2);
						size_t bracket4 = line.find(']', bracket3);
						std::string line_embedding = line.substr(bracket3 + 1, bracket4 - bracket3 - 1);
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
						std::string word_name = line.substr(0, bracket1);
						mat* MatObj_temp = new mat(word_name, next, embeddings);
						mats.push_back(MatObj_temp);
						contents += "\n" + line;
					}
				}
				else{
					/* a = fetching word, b = fetching next words, c = fetching embeddings, d = create mat */
					char state = 'a';
					bin binary;
					unsigned char byte;
					bin word;
					std::vector<bin> next;
					std::vector<float> embeddings;
					int ch;
					while((ch = file_in.get()) != EOF){
						byte = static_cast<unsigned char>(ch);
						switch(state){
							case 'a':
							if(byte == 0){
								break;
							}
								if(byte != 0x1F){
									binary.push_back(byte);
								}
								else{
									word = ::FetchWord(&binary);
									binary.clear();
									state = 'b';
								}
								break;
								
								case 'b':
								if(byte != 0x1F){
									binary.push_back(byte);
								}
								else{
									next = ::FetchNext(&binary);
									binary.clear();
									state = 'c';
								}
								break;
								
								case 'c':
								if(byte != 0x1F){
									binary.push_back(byte);
								}
								else{
									for(const auto& embedding: ::FetchEmbeddings(&binary)){
										embeddings.push_back(embedding);
									}
									binary.clear();
									state = 'd';
								}
								break;
							
								case 'd':
								mat* MatObj_temp = new mat(word, next, embeddings);
								mats.push_back(MatObj_temp);
								binary.push_back(byte);
								state = 'a';
								embeddings.clear();
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
		}
		static void roll(const std::string& path){;
			if(mode){
				std::ofstream file_out(path, std::ios::binary);
				file_out.write(reinterpret_cast<const char*>(&JesusByte), 1);
				for(const auto& word : mats){
					file_out.write(reinterpret_cast<const char*>(std::visit([&](const auto& data){return data.data();}, word->GetWord()),
					 std::visit([&](const auto& data){return data.size();}, word->GetWord())));
					file_out.put(0x1F);
					
					for(const auto& NextWord : word->GetNext()){
						file_out.write(reinterpret_cast<const char*>(NextWord.data()), NextWord.size());
						file_out.put(0);
					}
					file_out.put(0x1F);
					
					short i = 0;
					for(const auto& embedding : word->GetEmbeddings()){
						file_out.write(reinterpret_cast<const char*>(&embedding), sizeof(float));
					}
					file_out.put(0x1F);
				}
				file_out.put(0x4);
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
		}
		static std::tuple<short, unsigned char, bool, bool> inspect(){return {dims, JesusByte, mode, report};}
		static bool here(const std::string& target){
			if(mode){
				return (std::find_if(mats.begin(), mats.end(), 
				 [&](const auto& word){return std::string(word.GetWord().begin(), word.GetWord().end()) == target;}) != mats.end());
			}
			else{
				return (std::find_if(mats.begin(), mats.end(),
				 [&](const auto& word){return word.GetWord() == target;}) != mats.end());
			}
		}
		static void trash(const std::string& path){
			std::ofstream file_out(path);
			file_out.close();
		}

		std::tuple<mat_WordBoth, std::vector<std::string>, std::vector<float>> see(){return {this->word, this->next, this->embeddings};}
		const mat_WordBoth& GetWord(){return this->word;}
		const std::vector<mat_WordBoth>& GetNext(){return this->next;}
		const std::vector<float>& GetEmbeddings(){return this->embeddings;}
		void restyle(char attr, std::string assign, short index = -1){
			if(attr == 'w'){this->word = assign;}
			else if(attr == 'n' && index != -1){this->next[index] = assign;}
			else if(attr == 'e'&& index != -1){this->embeddings[index] = static_cast<short>(std::stoi(assign));}
			else{return;}
		}
		void sow(std::string append){this->next.push_back(append);}
		void trim(short index){this->next.erase(this->next.begin() + index);}

		mat(mat_WordBoth w, mat_NextBoth n, std::vector<float> e){
			word = w;
			next = n;
			embeddings = e;
		}
};

// these fetch functions are for binary mode
std::vector<unsigned char> FetchWord(std::vector<unsigned char>* binary){
	std::vector<unsigned char> result;
	for(const auto& byte : *binary){
		if(byte == 0x0 || byte == 0x1F){
			break;
		}
		result.push_back(byte);
	}
	return result;
}
std::vector<std::vector<unsigned char>> FetchNext(std::vector<unsigned char>* binary){
	std::vector<std::vector<unsigned char>> result;
	std::vector<unsigned char> word;
	for(const auto& byte : *binary){
		if(byte == 0x0){
			if(!word.empty()){
				result.push_back(word);
			}
			word.clear();
		}
		else if(byte == 0x1F){
			break;
		}
		else{
			word.push_back(byte);
		}
	}

	return result;
}
std::vector<float> FetchEmbeddings(std::vector<unsigned char>* binary){
	std::vector<float> result;
	std::vector<unsigned char> embedding_raw;
	float embedding;
	std::cout << "FetchEmbeddings binary size: " << binary->size() << std::endl;
	for(const auto& byte : *binary){
		embedding_raw.push_back(byte);
		std::cout << embedding_raw.size() << '\n';
		if(embedding_raw.size() == 4){
			std::memcpy(&embedding, embedding_raw.data(), sizeof(float));
			result.push_back(embedding);
			embedding_raw.clear();
		}
	}
	embedding_raw.clear();
	short diff = mat::dims - result.size();
	if(diff != 0){
		for(short _ = 0 ; _ < diff ; _++){
			result.push_back(0.0f);
		}
	}
	return result;
}

int main(){

	mat::lay("file.flrd");
	std::cout << '\n' << mat::here("obama") << ' ';
	std::tuple<short, unsigned char, bool, bool> inspected = mat::inspect();
	std::cout << '\n' << std::get<0>(inspected) << std::get<1>(inspected) << std::get<2>(inspected) << std::get<3>(inspected);
	std::cout << '\n' << std::bitset<8>(mat::JesusByte) << "\n";
	std::cout << '\n' << mat::mode << " | " << (int)mat::dims << "\n";
	std::cout << '\n' << mat::contents << "\n";
	for(const auto& thing : mat::mats){
		std::cout << thing << ":";
		if(mode){
			std::cout << reinterpret_cast<const char*>(thing->GetWord().data()) << "\n";
		}
		else{
			std::cout << thing->GetWord();
		}
		for(const auto& next : thing->GetNext()){
			for(const auto& ch : next){
				std::cout << ch;
			}
			std::cout << ",";
		}
		for(const auto& embedding : thing->GetEmbeddings()){
			std::cout << embedding << ",";
		}
		std::cout << "\n";
	}
	
	if(mat::mode){
		std::cout << "binary mode";
	}
	else{
		std::cout << "text mode";
	}

	if(mat::mats){
		std::cout << "nothing";
	}

	mat::roll("file.flrd");

	return 0;
}