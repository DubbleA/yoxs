// object.cpp
#include "object.hpp"
#include "../ast/ast.hpp"
#include <sstream>

namespace YOXS_OBJECT {

std::string Function::Inspect() const {
    std::ostringstream out;

    std::vector<std::string> params;
    for(const auto& param : Parameters) {
        params.push_back(param->String());
    }

    out << "fn(" << YOXS_AST::join(params, ", ") << ") {\n";
    out << Body->String() << "\n}";

    return out.str();
}

} // namespace YOXS_OBJECT