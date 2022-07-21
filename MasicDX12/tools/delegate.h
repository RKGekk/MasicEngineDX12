#pragma once

#include <tuple>
#include <functional>
#include <utility>

template<typename... Type>
struct type_list {
    using type = type_list;
    static constexpr auto size = sizeof...(Type);
};

template<std::size_t, typename>
struct type_list_element;

template<std::size_t Index, typename Type, typename... Other>
struct type_list_element<Index, type_list<Type, Other...>> : type_list_element<Index - 1u, type_list<Other...>> {};

template<std::size_t Index, typename List>
using type_list_element_t = typename type_list_element<Index, List>::type;

template<typename To, typename From>
struct constness_as {
    using type = std::remove_const_t<To>;
};

template<typename To, typename From>
using constness_as_t = typename constness_as<To, From>::type;

namespace internal {
    template<typename Ret, typename... Args>
    auto function_pointer(Ret(*)(Args...))->Ret(*)(Args...);

    template<typename Ret, typename Type, typename... Args, typename Other>
    auto function_pointer(Ret(*)(Type, Args...), Other&&)->Ret(*)(Args...);

    template<typename Class, typename Ret, typename... Args, typename... Other>
    auto function_pointer(Ret(Class::*)(Args...), Other &&...)->Ret(*)(Args...);

    template<typename Class, typename Ret, typename... Args, typename... Other>
    auto function_pointer(Ret(Class::*)(Args...) const, Other &&...)->Ret(*)(Args...);

    template<typename Class, typename Type, typename... Other>
    auto function_pointer(Type Class::*, Other &&...)->Type(*)();

    template<typename... Type>
    using function_pointer_t = decltype(internal::function_pointer(std::declval<Type>()...));

    template<typename... Class, typename Ret, typename... Args>
    [[nodiscard]]
    constexpr auto index_sequence_for(Ret(*)(Args...)) {
        return std::index_sequence_for<Class..., Args...>{};
    }
}

template<auto>
struct connect_arg_t {};

template<auto Func>
inline constexpr connect_arg_t<Func> connect_arg{};

template<typename>
class delegate;

template<typename Ret, typename... Args>
class delegate<Ret(Args...)> {
    template<auto Candidate, std::size_t... Index>
    [[nodiscard]]
    auto wrap(std::index_sequence<Index...>) noexcept {
        return [](const void*, Args... args) -> Ret {
            [[maybe_unused]]
            const auto arguments = std::forward_as_tuple(std::forward<Args>(args)...);
            return static_cast<Ret>(std::invoke(Candidate, std::forward<type_list_element_t<Index, type_list<Args...>>>(std::get<Index>(arguments))...));
        };
    }

    template<auto Candidate, typename Type, std::size_t... Index>
    [[nodiscard]]
    auto wrap(Type&, std::index_sequence<Index...>) noexcept {
        return [](const void* payload, Args... args) -> Ret {
            [[maybe_unused]]
            const auto arguments = std::forward_as_tuple(std::forward<Args>(args)...);
            Type* curr = static_cast<Type*>(const_cast<constness_as_t<void, Type> *>(payload));
            return static_cast<Ret>(std::invoke(Candidate, *curr, std::forward<type_list_element_t<Index, type_list<Args...>>>(std::get<Index>(arguments))...));
        };
    }

    template<auto Candidate, typename Type, std::size_t... Index>
    [[nodiscard]]
    auto wrap(Type*, std::index_sequence<Index...>) noexcept {
        return [](const void* payload, Args... args) -> Ret {
            [[maybe_unused]]
            const auto arguments = std::forward_as_tuple(std::forward<Args>(args)...);
            Type* curr = static_cast<Type*>(const_cast<constness_as_t<void, Type> *>(payload));
            return static_cast<Ret>(std::invoke(Candidate, curr, std::forward<type_list_element_t<Index, type_list<Args...>>>(std::get<Index>(arguments))...));
        };
    }

public:
    using function_type = Ret(const void*, Args...);
    using type = Ret(Args...);
    using result_type = Ret;

    delegate() noexcept : fn{ nullptr }, data{ nullptr } {}

    template<auto Function>
    delegate(connect_arg_t<Function>) : delegate{} {
        connect<Function>();
    }

    template<auto Candidate, typename Type>
    delegate(connect_arg_t<Candidate>, Type&& value_or_instance) noexcept {
        connect<Candidate>(std::forward<Type>(value_or_instance));
    }

    delegate(function_type* function, const void* payload = nullptr) noexcept {
        connect(function, payload);
    }

    template<auto Candidate>
    void connect() noexcept {
        data = nullptr;
        if constexpr (std::is_invocable_r_v<Ret, decltype(Candidate), Args...>) {
            fn = [](const void*, Args... args) -> Ret {
                return Ret(std::invoke(Candidate, std::forward<Args>(args)...));
            };
        }
        else if constexpr (std::is_member_pointer_v<decltype(Candidate)>) {
            fn = wrap<Candidate>(internal::index_sequence_for<type_list_element_t<0, type_list<Args...>>>(internal::function_pointer_t<decltype(Candidate)>{}));
        }
        else {
            fn = wrap<Candidate>(internal::index_sequence_for(internal::function_pointer_t<decltype(Candidate)>{}));
        }
    }

    template<auto Candidate, typename Type>
    void connect(Type& value_or_instance) noexcept {
        data = &value_or_instance;
        if constexpr (std::is_invocable_r_v<Ret, decltype(Candidate), Type&, Args...>) {
            fn = [](const void* payload, Args... args) -> Ret {
                Type* curr = static_cast<Type*>(const_cast<constness_as_t<void, Type> *>(payload));
                return Ret(std::invoke(Candidate, *curr, std::forward<Args>(args)...));
            };
        }
        else {
            fn = wrap<Candidate>(value_or_instance, internal::index_sequence_for(internal::function_pointer_t<decltype(Candidate), Type>{}));
        }
    }

    template<auto Candidate, typename Type>
    void connect(Type* value_or_instance) noexcept {
        data = value_or_instance;
        if constexpr (std::is_invocable_r_v<Ret, decltype(Candidate), Type*, Args...>) {
            fn = [](const void* payload, Args... args) -> Ret {
                Type* curr = static_cast<Type*>(const_cast<constness_as_t<void, Type> *>(payload));
                return Ret(std::invoke(Candidate, curr, std::forward<Args>(args)...));
            };
        }
        else {
            fn = wrap<Candidate>(value_or_instance, internal::index_sequence_for(internal::function_pointer_t<decltype(Candidate), Type>{}));
        }
    }

    void connect(function_type* function, const void* payload = nullptr) noexcept {
        fn = function;
        data = payload;
    }

    void reset() noexcept {
        fn = nullptr;
        data = nullptr;
    }

    [[nodiscard]]
    const void* instance() const noexcept {
        return data;
    }

    Ret operator()(Args... args) const {
        return fn(data, std::forward<Args>(args)...);
    }

    [[nodiscard]]
    explicit operator bool() const noexcept {
        return !(fn == nullptr);
    }

    [[nodiscard]]
    bool operator==(const delegate<Ret(Args...)>& other) const noexcept {
        return fn == other.fn && data == other.data;
    }

private:
    function_type* fn;
    const void* data;
};

template<typename Ret, typename... Args>
[[nodiscard]]
bool operator!=(const delegate<Ret(Args...)>& lhs, const delegate<Ret(Args...)>& rhs) noexcept {
    return !(lhs == rhs);
}

template<auto Candidate>
delegate(connect_arg_t<Candidate>) noexcept -> delegate<std::remove_pointer_t<internal::function_pointer_t<decltype(Candidate)>>>;

template<auto Candidate, typename Type>
delegate(connect_arg_t<Candidate>, Type&&) noexcept -> delegate<std::remove_pointer_t<internal::function_pointer_t<decltype(Candidate), Type>>>;

template<typename Ret, typename... Args>
delegate(Ret(*)(const void*, Args...), const void* = nullptr) noexcept -> delegate<Ret(Args...)>;