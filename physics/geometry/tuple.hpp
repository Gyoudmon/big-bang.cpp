#pragma once

#include <stdexcept>

#include "../../datum/flonum.hpp"
#include "../../datum/string.hpp"

namespace WarGrey::STEM {
    template<template<typename> class Child, typename T>
    class __lambda__ Tuple {
    public:
        Tuple() : Tuple(T(), T()) {}
        Tuple(T x, T y) : x(x), y(y) {}
       
        Tuple(const Child<T>& c) {
            this->x = c.x;
            this->y = c.y;
        }

        Tuple(const Child<T>&& c) {
            this->x = c.x;
            this->y = c.y;
        }

        Child<T>& operator=(const Child<T>& c) {
            this->x = c.x;
            this->y = c.y;
           
            return static_cast<Child<T>&>(*this);
        }

        Child<T>& operator=(Child<T>&& c) {
            this->x = c.x;
            this->y = c.y;
           
            return static_cast<Child<T>&>(*this);
        }

        ~Tuple() noexcept {}

    public:
        bool is_zero() const { return (this->x == T(0)) && (this->y == T(0)); }
        bool has_nan() const { return flisnan(this->x) || flisnan(this->y); }

        bool operator==(const Child<T>& c) const { return this->x == c.x && this->y == c.y; }
        bool operator!=(const Child<T>& c) const { return this->x != c.x || this->y != c.y; }

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
        
        Child<T> operator-() const { return { -this->x, -this->y }; }

    public:
        template<typename U> auto operator+(const Child<U>& c) const -> Child<decltype(T{} + U{})> { return { this->x + c.x, this->y + c.y }; }
        template<typename U> auto operator-(const Child<U>& c) const -> Child<decltype(T{} - U{})> { return { this->x - c.x, this->y - c.y }; }
        template<typename U> auto operator*(U s) const -> Child<decltype(T{} * U{})> { return { this->x * s, this->y * s }; }
        template<typename U> auto operator/(U d) const -> Child<decltype(T{} / U{})> { return { this->x / d, this->y / d }; }

        template <typename U>
        Child<T>& operator+=(const Child<U>& c) {
            this->x += c.x;
            this->y += c.y;
           
            return static_cast<Child<T>&>(*this);
        }
       
        template <typename U>
        Child<T>& operator-=(const Child<U>& c) {
            this->x -= c.x;
            this->y -= c.y;
            
            return static_cast<Child<T>&>(*this);
        }
       
        template <typename U>
        Child<T>& operator*=(U s) {
            this->x *= s;
            this->y *= s;
            
            return static_cast<Child<T>&>(*this);
        }

        template <typename U>
        Child<T>& operator/=(U d) {
            this->x /= d;
            this->y /= d;
           
            return static_cast<Child<T>&>(*this);
        }

    public:
        std::string desc() const {
            return make_nstring("(%s, %s)", std::to_string(x).c_str(), std::to_string(y).c_str());
        }

    public:
        T x;
        T y;
    };
}
