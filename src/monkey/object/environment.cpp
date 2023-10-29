// environment.cpp
#include "environment.hpp"

std::shared_ptr<Object> Environment::Get(const std::string& name) {
    auto it = store.find(name);
    if(it != store.end()) {
        return it->second;
    } else if(outer != nullptr) {
        return outer->Get(name);
    } else {
        return nullptr;  // or throwing an exception
    }
}

std::shared_ptr<Object> Environment::Set(const std::string& name, std::shared_ptr<Object> val) {
    store[name] = val;
    return val;
}