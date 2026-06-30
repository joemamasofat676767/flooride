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

std::vector<unsigned char> FetchWord(std::vector<unsigned char>* binary);
std::vector<std::vector<unsigned char>> FetchNext(std::vector<unsigned char>* binary);
std::vector<float> FetchEmbeddings(std::vector<unsigned char>* binary);

template<typename type>
class mat{
	private:
	type word;
	std::vector<type> next;
	std::vector<float> embeddings;

	public:
	
	inline static short dims;
	inline static unsigned char JesusByte;
	inline static bool mode;
	inline static bool report;
	inline static std::vector<mat<std::vector<unsigned char>>*> mats_bin;
	inline static std::vector<mat<std::string>*> mats_txt;
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
						mat<std::string>* MatObj_temp = new mat<std::string>(word_name, next, embeddings);
						mats_txt.push_back(MatObj_temp);
						contents += "\n" + line;
					}
				}
				else{
					/* a = fetching word, b = fetching next words, c = fetching embeddings, d = create mat */
					char state = 'a';
					std::vector<unsigned char> binary;
					unsigned char byte;
					std::vector<unsigned char> word;
					std::vector<std::vector<unsigned char>> next;
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
								mat<std::vector<unsigned char>>* MatObj_temp = new mat<std::vector<unsigned char>>(word, next, embeddings);
								mats_bin.push_back(MatObj_temp);
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
				for(const auto& data : mats_bin){
					file_out.write(reinterpret_cast<const char*>(data->word.data()), data->word.size());
					file_out.put(0x1F);
					
					for(const auto& NextWord : data->next){
						file_out.write(reinterpret_cast<const char*>(NextWord.data()), NextWord.size());
						file_out.put(0);
					}
					file_out.put(0x1F);
					
					short i = 0;
					for(const auto& embedding : data->embeddings){
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
				return (std::find_if(mats_bin.begin(), mats_bin.end(), 
				 [&](const auto& word){return std::string(word->word.begin(), word->word.end()) == target;}) != mats_bin.end());
			}
			else{
				return (std::find_if(mats_txt.begin(), mats_txt.end(),
				 [&](const auto& word){return word->word == target;}) != mats_txt.end());
			}
		}
		static void trash(const std::string& path){
			std::ofstream file_out(path);
			file_out.close();
		}
	// 	inline static short dims;
	// inline static unsigned char JesusByte;
	// inline static bool mode;
	// inline static bool report;
	// inline static std::vector<mat<std::vector<unsigned char>>*> mats_bin;
	// inline static std::vector<mat<std::string>*> mats_txt;
	// inline static std::string contents;
		static void paint(std::string target, std::string assign){
			switch(target){
				case "dims":
					for(const auto& word : ((mode) ? mats_bin : mats_txt)){
						word->sow();
					}
					break;
				case "":

					break;
				case "":

					break;
				case "":

					break;
				case "":

					break;
				case "":

					break;
				case "":

					break;
			}
		}

		std::tuple<type, std::vector<std::string>, std::vector<float>> inspect(){return {this->word, this->next, this->embeddings};}
		type GetWord(){return this->word;}
		std::vector<std::string> GetNext(){return this->next;}
		std::vector<float> GetEmbeddings(){return this->embeddings;}
		void restyle(std::string target , char attr, std::string assign, short index = -1){
			if(!mat<std::string>::here(target)){return;}

			if(attr == 'w'){this->word = assign;}
			else if(attr == 'n'){(index != -1) ? this->next[index] = target : return;}
			else if(attr == 'e'){(index != -1) ? this->embeddings[index] = reinterpret_cast<short>(target) : return;}
			else{return;}
		}
		void sow(std::string append){this->next.pushback(append);}
		void trim(short index){this->next.erase(tthis-next.begin() + index);}
		mat(type w, std::vector<type> n, std::vector<float> e){
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
	for(const auto& byte : *binary){	
		std::cout << std::hex << (int)byte << ' ';
		embedding_raw.push_back(byte);
		if(embedding_raw.size() == 4){
			std::memcpy(&embedding, embedding_raw.data(), sizeof(float));
			result.push_back(embedding);
			embedding_raw.clear();
		}
	}
	return result;
}

int main(){

	mat<std::string>::lay("file.flrd");
	std::cout << '\n' << mat<std::string>::here("obama") << ' ';
	std::tuple inspected = mat<std::string>::inspect();
	std::cout << '\n' << std::get<0>(inspected) << std::get<1>(inspected) << std::get<2>(inspected) << std::get<3>(inspected);
	std::cout << '\n' << std::bitset<8>(mat<std::string>::JesusByte) << "\n";
	std::cout << '\n' << mat<std::string>::mode << " | " << (int)mat<std::string>::dims << "\n";
	std::cout << '\n' << mat<std::string>::contents << "\n";
	for(const auto& thing : mat<std::string>::mats_bin){
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
	
	if(mat<std::string>::mode){
		std::cout << "binary mode";
	}
	else{
		std::cout << "text mode";
	}

	if(mat<std::string>::mats_bin.empty() && mat<std::string>::mats_txt.empty()){
		std::cout << "nothing";
	}

	mat<std::string>::roll("file.flrd");

	return 0;
}