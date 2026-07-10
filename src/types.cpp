#include "../headers/types.h"
#include <string>

Type* Type::get_real_type(const std::string& s){
    auto find_result = Types::type_keywords.find(s);
    if(find_result == Types::type_keywords.end()){
        auto return_type = new ClassType(s, {});
        return return_type;
    }
    if(s == "int"){
        auto return_type = new IntType(0);
        return return_type;
    }
    else if(s == "boolean"){
       auto return_type = new BooleanType(false);
       return return_type; 
    }
    else if(s == "int[]"){
        auto return_type = new IntArrayType(16);
        return return_type;
    }
    return nullptr;
}
void ClassType::insert_all_fields(std::unordered_map<std::string, Type*> t){
    fields = std::move(t);
    this->byte_width = 0;
    for(auto types: fields){
        this->byte_width += types.second->byte_width;
    }
}

void IntArrayType::reserve_new_size(size_t sz){
    size = sz;
    byte_width = size*8;
    arr.reserve(sz);
}

std::string Type::get_type_as_string(){
    switch (this->real_type) {

    case INT:
        return "int";
    case BOOL:
        return "boolean";
    case INT_ARR:
        return "int[]";
    case CLASS_TYPE:
        auto ptr = static_cast<ClassType*>(this);
        return ptr->class_name;
      break;
    }
}