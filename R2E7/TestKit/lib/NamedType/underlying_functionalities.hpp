#ifndef UNDERLYING_FUNCTIONALITIES_HPP
#define UNDERLYING_FUNCTIONALITIES_HPP

#include "crtp.hpp"
#include "named_type_impl.hpp"

#include <functional>
#include <memory>

namespace fluent
{

template <typename T>
struct Incrementable : crtp<T, Incrementable>
{
    T& operator+=(T const& other) { this->underlying().get() += other.get(); return this->underlying(); }
};

template <typename T>
struct PreIncrementable : crtp<T, PreIncrementable>
{
    T& operator++() { ++this->underlying().get(); return this->underlying(); }
};

template <typename T>
struct Addable : crtp<T, Addable>
{
    T operator+(T const& other) const { return T(this->underlying().get() + other.get()); }
};

template <typename T>
struct Subtractable : crtp<T, Subtractable>
{
    T operator-(T const& other) const { return T(this->underlying().get() - other.get()); }
};

template <typename T>
struct Multiplicable : crtp<T, Multiplicable>
{
    T operator*(T const& other) const { return T(this->underlying().get() * other.get()); }
};

template <typename T>
struct Negatable : crtp<T, Negatable>
{
    T operator-() const { return T(-this->underlying().get()); }
};

template <typename T>
struct Comparable : crtp<T, Comparable>
{
    bool operator<(T const& other) const  { return this->underlying().get() < other.get(); }
    bool operator>(T const& other) const  { return other.get() < this->underlying().get(); }
    bool operator<=(T const& other) const { return !(*this > other); }
    bool operator>=(T const& other) const { return !(*this < other); }
    bool operator==(T const& other) const { return !(*this < other || *this > other); }
    bool operator!=(T const& other) const { return !(*this == other); }
};

struct Operable : Addable, Subtractable, Multiplicable, Negatable, Comparable{};

template <typename Destination>
struct ImplicitlyConvertibleTo
{
    template <typename T>
    struct templ : crtp<T, templ>
    {
        operator Destination() const
        {
            return this->underlying().get();
        }
    };
};

template<typename NamedType_>
struct FunctionCallable;

template <typename T, typename Parameter, template<typename> class... Skills>
struct FunctionCallable<NamedType<T, Parameter, Skills...>> : crtp<NamedType<T, Parameter, Skills...>, FunctionCallable>
{
    operator T const&() const
    {
        return this->underlying().get();
    }
    operator T&()
    {
        return this->underlying().get();
    }
};

template<typename NamedType_>
struct MethodCallable;

template <typename T, typename Parameter, template<typename> class... Skills>
struct MethodCallable<NamedType<T, Parameter, Skills...>> : crtp<NamedType<T, Parameter, Skills...>, MethodCallable>
{
    T const* operator->() const { return etl::addressof(this->underlying().get()); }
    T* operator->() { return etl::addressof(this->underlying().get()); }
};

template<typename NamedType_>
struct Callable : FunctionCallable<NamedType_>, MethodCallable<NamedType_>{};

} // namespace fluent


#endif
