#define JSON_IMPLEMENTATION
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include <vector>
#include <array>
#include "mmx_json.h"

typedef struct json_t json_t;

struct MatStruct{
	std::vector<std::string> next;
	std::array<float, 3> embedding;
};
using mat = std::unordered_map<std::string, MatStruct>;

class mats{
	public:
		mat _knowledge;
	private:
		std::string name;
		
	public:

		void setSrc(std::string path){
			std::ifstream file(path);
			if (!file.is_open()){
				throw std::runtime_error("cant open file: " + path);
			}
			std::string content((std::istreambuf_iterator<char>(file)),
								std::istreambuf_iterator<char>());
			std::cout << content.substr(0, 500);
			file.clear();
			file.seekg(0);
			
			json_t mat_json;
			json_parse(&mat_json, content.c_str());
			file.close();
			
			for (int i = 0 ; i < mat_json.size ; i++){
				auto*  WordData = &mat_json.items[i];
				std::cout << WordData->value;
				std::vector<std::string> next = {};
				if (WordData->value->type == JSON_ARRAY && WordData->value->size > 0){
					for (int j = 0 ; j < WordData->value->size ; j++){
						next.push_back(WordData->value.items[0].value->items[j].value->string_value);
					}
				}
				_knowledge[WordData->key] = {
					next.size() > 0 ? next : std::vector<std::string>(),
					{WordData->value->items[1].value->items[0].value->number_value,
					 WordData->value->items[1].value->items[1].value->number_value,
				 	 WordData->value->items[1].value->items[2].value->number_value}
				};
			}
			json_free(&mat_json);
		}

		MatStruct lay(const std::string& name){
			return _knowledge[name];
		}
};	

PYBIND11_MODULE(flooride, m){
	pybind11::class_<MatStruct>(m, "MatStruct")
		.def(pybind11::init<>())
		.def_readwrite("next", &MatStruct::next)
		.def_readwrite("embedding", &MatStruct::embedding);
	pybind11::class_<mats>(m,"mats")
		.def(pybind11::init<>())
		.def("setSrc", &mats::setSrc)
		.def("lay", &mats::lay)
		.def_readwrite("_knowledge", &mats::_knowledge);
}
