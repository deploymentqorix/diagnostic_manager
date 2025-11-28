#ifndef ARA_CORE_INSTANCE_SPECIFIER_H_
#define ARA_CORE_INSTANCE_SPECIFIER_H_

#include <string>

namespace ara {
namespace core {

class InstanceSpecifier {
public:
    InstanceSpecifier() = default;
    explicit InstanceSpecifier(std::string name) : name_(std::move(name)) {}
    const std::string &GetName() const noexcept { return name_; }

private:
    std::string name_;
};

} // namespace core
} // namespace ara

#endif // ARA_CORE_INSTANCE_SPECIFIER_H_