// object.cpp
#include "object.hpp"
#include <sstream>

namespace YOXS_OBJECT {

std::string Function::Inspect() const {
    std::ostringstream out;

    std::vector<std::string> params;
    for(const auto& param : Parameters) {
        params.push_back(param->String());
    }

    out << "fn(" << join(params, ", ") << ") {\n";
    out << Body->String() << "\n}";

    return out.str();
}

std::string join(const std::vector<std::string>& vec, const std::string& delimiter) {
    std::ostringstream result;
    for(size_t i = 0; i < vec.size(); ++i) {
        result << vec[i];
        if(i < vec.size() - 1) {
            result << delimiter;
        }
    }
    return result.str();
}

} // namespace YOXS_OBJECT