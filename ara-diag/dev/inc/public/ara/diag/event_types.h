#ifndef ARA_DIAG_EVENT_TYPES_H_
#define ARA_DIAG_EVENT_TYPES_H_

#include <cstdint>
#include <type_traits>
#include <initializer_list>
#include <iostream>
#include <thread>
#include <chrono>

namespace ara {
namespace diag {

enum class EventStatusBit : std::uint8_t {
    FailedAndTested = 0x01,
    PassedAndTested = 0x02,
    // additional bits may be added later
};

struct EventStatusByte {
    std::uint8_t value{0};

    template <typename... Args>
    constexpr EventStatusByte(Args... bits) noexcept
        : value{0} {
        std::cout << __func__ << ":" << __LINE__ << std::endl;
        (void)std::initializer_list<int>{(value |= static_cast<std::uint8_t>(bits), 0)...};
    }

    constexpr bool IsFailedAndTested() const noexcept {
        std::cout << __func__ << ":" << __LINE__ << std::endl;
        return (value & static_cast<std::uint8_t>(EventStatusBit::FailedAndTested)) != 0;
    }

    constexpr bool IsPassedAndTested() const noexcept {
        std::cout << __func__ << ":" << __LINE__ << std::endl;
        return (value & static_cast<std::uint8_t>(EventStatusBit::PassedAndTested)) != 0;
    }

    template <typename... Args>
    constexpr bool IsSet(Args... bits) const noexcept {
        std::uint8_t mask = 0;
        (void)std::initializer_list<int>{(mask |= static_cast<std::uint8_t>(bits), 0)...};
        return (value & mask) == mask;
    }

    template <typename... Args>
    constexpr bool IsNotSet(Args... bits) const noexcept {
        std::uint8_t mask = 0;
        (void)std::initializer_list<int>{(mask |= static_cast<std::uint8_t>(bits), 0)...};
        return (value & mask) == 0;
    }
};

} // namespace diag
} // namespace ara

#endif // ARA_DIAG_EVENT_TYPES_H_

