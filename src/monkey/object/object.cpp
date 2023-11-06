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

std::string ObjectTypeToString(ObjectType type) {
    switch (type) {
        case NULL_OBJ: return "NULL";
        case ERROR_OBJ: return "ERROR";
        case INTEGER_OBJ: return "INTEGER";
        case BOOLEAN_OBJ: return "BOOLEAN";
        case RETURN_VALUE_OBJ: return "RETURN_VALUE";
        case FUNCTION_OBJ: return "FUNCTION";
        default: return "UNKNOWN";
    }
}

} // namespace YOXS_OBJECT