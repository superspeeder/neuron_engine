#pragma once

#include "pre.hpp"
#include "utils.hpp"
#include <concepts>

namespace neuron::stem {
    template<typename T>
    class Provider {
    public:
        virtual ~Provider() = default;
        virtual const T& provide(blank_typed<T>) = 0;
    };

    template<typename T, std::derived_from<Provider<T>> P>
    inline const T& provide(P* const provider) {
        return reinterpret_cast<Provider<T>*>(provider)->provide(blank_typed<T>{});
    };

    template<typename T, typename P>
    inline const T& provide(P* const provider) requires(!std::derived_from<Provider<T>, P>) {
        return dynamic_cast<Provider<T>*>(provider)->provide(blank_typed<T>{});
    };

    template<typename T, typename P>
    inline const T& provide(const std::shared_ptr<P>& sp) {
        return provide<T, P>(sp.get());
    };

    template<typename T, typename P>
    inline const T& provide(const std::unique_ptr<P>& sp) {
        return provide<T, P>(sp.get());
    };
}