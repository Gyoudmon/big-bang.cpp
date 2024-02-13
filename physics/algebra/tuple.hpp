#pragma once

#include <stdexcept>
#include <string>

#include "../../datum/flonum.hpp"

namespace GYDM {
    template<template<typename> class Child, typename T>
    class __lambda__ Tuple {
    public:
        Tuple() noexcept : Tuple(T(), T()) {}
        Tuple(T x, T y) noexcept : x(x), y(y) {}
        Tuple(const Child<T>& c) noexcept : x(c.x), y(c.y) {}

        Child<T>& operator=(const Child<T>& c) noexcept {
            this->x = c.x;
            this->y = c.y;
           
            return static_cast<Child<T>&>(*this);
        }

        ~Tuple() noexcept {}

    public:
        bool is_zero() const noexcept { return (this->x == T(0)) && (this->y == T(0)); }
        bool has_nan() const noexcept { return flisnan(this->x) || flisnan(this->y); }
        bool okay() const noexcept { return !this->has_nan(); }

        bool operator==(const Child<T>& c) const noexcept { return (this->x == c.x) && (this->y == c.y); }
        bool operator!=(const Child<T>& c) const noexcept { return (this->x != c.x) || (this->y != c.y); }

    public:
        T operator[](size_t i) const {
            switch (i) {
            case 0: return this->x;
            case 1: return this->y;
            default: throw std::out_of_range("index too large for a 2D Tuple");
            }
        }
       
       
        T& operator[](size_t i) {
            switch (i) {
            case 0: return this->x;
            case 1: return this->y;
            default: throw std::out_of_range("index too large for a 2D Tuple");
            }
        }
        
        Child<T> operator-() const noexcept { return { -this->x, -this->y }; }

    public:
        Child<T> operator+(const Child<T>& c) const noexcept { return { this->x + c.x, this->y + c.y }; }
        Child<T> operator-(const Child<T>& c) const noexcept { return { this->x - c.x, this->y - c.y }; }
        Child<T> operator*(T s) const noexcept { return { this->x * s, this->y * s }; }
        Child<T> operator/(T d) const noexcept { return { this->x / d, this->y / d }; }

        friend inline Child<T> operator+(T lhs, const Child<T>& rhs) noexcept { return rhs *= lhs; }

        Child<T>& operator+=(const Child<T>& c) noexcept {
            this->x += c.x;
            this->y += c.y;
           
            return static_cast<Child<T>&>(*this);
        }
       
        Child<T>& operator-=(const Child<T>& c) noexcept {
            this->x -= c.x;
            this->y -= c.y;
            
            return static_cast<Child<T>&>(*this);
        }
       
        Child<T>& operator*=(T s) noexcept {
            this->x *= s;
            this->y *= s;
            
            return static_cast<Child<T>&>(*this);
        }

        Child<T>& operator/=(T d) noexcept {
            this->x /= d;
            this->y /= d;
           
            return static_cast<Child<T>&>(*this);
        }

    public:
        std::string desc() const noexcept {
            return "(" + std::to_string(this->x) + ", " + std::to_string(this->y) + ")";
        }

    public:
        T x;
        T y;
    };
}
