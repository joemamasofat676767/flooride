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
#include <chrono>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>

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

	static void paint(char attr, short assign){
		if(attr == 'm'){
			mode = (assign > 0); 
			if(mode){
				JesusByte |= ((assign > 0) << 7);
				for(const auto& obj : mats){
					if(std::holds_alternative<std::string>(obj->word)){
						std::string& word = std::get<0>(obj->word);
						obj->word = bin(word.begin(), word.end());
						std::vector<bin> NewNext;
						for(const auto& NextWord : std::get<0>(obj->next)){
							NewNext.push_back(bin(NextWord.begin(), NextWord.end()));
						}
						obj->next = NewNext;
					}
				}
			}
			else{
				JesusByte &= ~((assign > 0) << 7);
				for(const auto& obj : mats){
					if(std::holds_alternative<bin>(obj->word)){
						bin& word = std::get<1>(obj->word);
						obj->word = std::string(word.begin(), word.end());
						std::vector<std::string> NewNext;
						for(const auto& NextWord : std::get<1>(obj->next)){
							NewNext.push_back(std::string(NextWord.begin(), NextWord.end()));
						}
						obj->next = NewNext;
					}
				}
			}
		}
		else if(attr == 'r'){report = (assign > 0);}
		else if(attr == 'd'){dims = assign; JesusByte = static_cast<unsigned char>(assign) | (mode << 7);}
		else{return;}
	}
	
	static void lay(const std::string& path){	
		std::ifstream file_in(path, std::ios::binary);
		
		// 1 = binary, 0 = text
		file_in.read(reinterpret_cast<char*>(&JesusByte), 1);
		if(file_in.is_open()){
			mode = (JesusByte >> 7) & 1;
			dims = JesusByte & 0b01111111;
			file_in.seekg(1);
			if(!mode){
				std::string line;
				bool FirstLine = true;
				while(std::getline(file_in, line)){
					if(FirstLine){FirstLine = false; continue;}
					
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
					MakeMat(word_name, next, embeddings);
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
						MakeMat(word, next, embeddings);
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
	static void roll(const std::string& path){
		if(mode){
			std::ofstream file_out(path, std::ios::binary);
			file_out.write(reinterpret_cast<const char*>(&JesusByte), 1);
			for(const auto& word : mats){
				std::visit([&](const auto& data){
					file_out.write(reinterpret_cast<const char*>(data.data()), data.size());
					file_out.put(0x1F);
				}, word->GetWord());
				
				const auto& NextWords = std::get<1>(word->GetNext());
				for(const auto& NextWord : NextWords){
					file_out.write(reinterpret_cast<const char*>(NextWord.data()), NextWord.size());
					file_out.put(0);
				}
				file_out.put(0x1F);
				
				for(const auto& embedding : word->GetEmbeddings()){
					file_out.write(reinterpret_cast<const char*>(&embedding), sizeof(float));
				}
				file_out.put(0x1F);
			}
			file_out.put(0x4);
			file_out.flush();
			file_out.close();
			
			for(auto* m : mats){
				delete m;
			}
		}
		else{
			std::ofstream file_out(path);
			file_out.write(reinterpret_cast<char*>(&JesusByte), 1);
			file_out.put(0x0A);
			for(const auto& obj : mats){
				std::vector<std::string> next = std::get<0>(obj->GetNext());
				std::string buffer = std::get<0>(obj->GetWord()) + "[";
				for(const auto& word : next){
					buffer += word + ",";
				}
				if(!next.empty()){
					buffer.pop_back();
				}
				buffer += "][";
				for(const auto& embedding : obj->GetEmbeddings()){
					buffer += std::to_string(embedding) + ",";
				}
				if(!obj->GetEmbeddings().empty()){
					buffer.pop_back();
				}
				buffer += "]\n";
				file_out.write(buffer.c_str(), buffer.size());
				buffer.clear();
			}
			file_out.flush();
			file_out.close();
		}
	}
	static std::tuple<short, unsigned char, bool, bool> inspect(){return {dims, JesusByte, mode, report};}
	static bool here(const std::string& target){
		if(mode){
			return std::find_if(mats.begin(), mats.end(),
			[&](const auto& word){return std::string(std::get<1>(word->GetWord()).begin(), std::get<1>(word->GetWord()).end()) == target;})
			!= mats.end();
		}
		else{
			return std::find_if(mats.begin(), mats.end(),
			[&](const auto& word){return std::get<0>(word->GetWord()) == target;}) != mats.end();
		}
	}
	static void trash(const std::string& path){
		std::ofstream file_out(path);
		file_out.close();
	}
	static mat* MakeMat(mat_WordBoth w, mat_NextBoth n, std::vector<float> e){
		mat* obj = new mat(w, n, e);
		mats.push_back(obj);
		return obj;
	}	

	std::tuple<mat_WordBoth, mat_NextBoth, std::vector<float>> see(){return {this->word, this->next, this->embeddings};}
	const mat_WordBoth& GetWord() const {return this->word;}
	const mat_NextBoth& GetNext() const {return this->next;}
	const std::vector<float>& GetEmbeddings() const {return this->embeddings;}
	void restyle(char attr, std::string assign, short index = -1){
		if(attr == 'w'){
			if(mode){
				this->word = bin(assign.begin(), assign.end());
			}
			else{
				this->word = assign;
			}
		}
		else if(attr == 'n' && index != -1){
			if(mode){
				std::get<1>(this->next)[index] = bin(assign.begin(), assign.end());
			}
			else{
				std::get<0>(this->next)[index] = assign;
			}
		}
		else if(attr == 'e'&& index != -1){this->embeddings[index] = static_cast<float>(std::stof(assign));}
		else{return;}
	}
	void sow(std::string append){
		if(mode){
			std::get<1>(this->next).push_back(bin(append.begin(), append.end()));
		}
		else{
			std::get<0>(this->next).push_back(append);
		}
	}
	void trim(short index){
		if(mode){
			std::get<0>(this->next).erase(std::get<0>(this->next).begin() + index);
		}
		else{
			std::get<1>(this->next).erase(std::get<1>(this->next).begin() + index);
		}
	}
	
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
	for(const auto& byte : *binary){
		embedding_raw.push_back(byte);
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

PYBIND11_MODULE(flooride, m){
	pybind11::class_<mat>(m, "mat")
	 .def_static("paint", &mat::paint, "change mat class attr")
	 .def_static("lay", &mat::lay, "loads target file")
	 .def_static("roll", &mat::roll, "write data to target file")
	 .def_static("inspect", &mat::inspect, "see mat class attr")
	 .def_static("here", &mat::here, "check is an mat obj exists")
	 .def_static("trash", &mat::trash, "clears target file")
	 .def("see", &mat::see, "look at a mat obj's word, next and embeddings")
	 .def("GetWord", &mat::GetWord, "gets word of a mat obj")
	 .def("GetNext", &mat::GetNext, "gets next of a mat obj")
	 .def("GetEmbeddings", &mat::GetEmbeddings, "gets embeddings of a mat obj")
	 .def("restyle", &mat::restyle, "changes an attr of a mat obj")
	 .def("sow", &mat::sow, "appeds a word to a mat obj")
	 .def("trim", &mat::trim, "deletes a word from mat obj by index")
	 .def("MakeMat", &mat::MakeMat, "makes a mat obj");
}