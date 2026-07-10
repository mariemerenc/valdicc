#pragma once
#include <cstddef>
#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace Types{
    inline const std::set<std::string> type_keywords = 
    {"int",
    "boolean",
    "int[]",
    };
    
};

struct Type{
    virtual ~Type() = default; 
    size_t byte_width;
    enum type_kind{
        INT,
        BOOL,
        INT_ARR,
        CLASS_TYPE
    } real_type;
    std::string get_type_as_string();
    static Type* get_real_type(const std::string& s);
};

struct IntType : public Type{
    
    int64_t val;
    IntType(int64_t v) : val{v}{
        byte_width = 8;
        real_type = type_kind::INT;
    }
};

struct BooleanType : public Type{
    bool val;
    BooleanType(bool b) : val{b}{
        byte_width = 1;
        real_type = type_kind::BOOL;
    }
};
struct IntArrayType : public Type{
    size_t size;
    std::vector<int64_t> arr;
    IntArrayType(size_t sz): size{sz}{
        byte_width = size*8;
        arr.reserve(sz);
    }
    void reserve_new_size(size_t sz);
};
struct ClassType : public Type{
    std::unordered_map<std::string,Type*> fields;
    std::string class_name;
    ClassType(std::string name ,std::unordered_map<std::string,Type*> t){
        class_name = name;
        fields = std::move(t);
        this->byte_width = 0;
        for(auto types: fields){
            this->byte_width += types.second->byte_width;
        }
    }
    void insert_all_fields(std::unordered_map<std::string, Type*> t);
};

