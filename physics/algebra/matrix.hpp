#pragma once

/**
 * Getting the semantics of 2D-Array right is hard, and might not be that of much an effort
 * 
 * Worthy of reading: https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0009r6.html
 **/

#include "../../datum/type.hpp"
#include "../../datum/array.hpp"
#include "../../datum/flonum.hpp"
#include "../../datum/string.hpp"

#include <type_traits>

namespace GYDM {
    /*********************************************************************************************/
    template<typename Fl, typename Super>
    Fl matrix_determinant(const Fl (&self)[1][1]) {
        return self[0][0];
    }

    template<typename Fl, typename Super>
    inline Super matrix_determinant(const Fl (&self)[2][2]) {
        return fl_safe_subtract_products<Super, Fl>(self[0][0], self[1][1], self[0][1], self[1][0]);
    }

    template<typename Fl, typename Super>
    Super matrix_determinant(const Fl (&self)[3][3]) {
        Super minor12 = fl_safe_subtract_products<Super, Fl>(self[1][1], self[2][2], self[1][2], self[2][1]);
        Super minor02 = fl_safe_subtract_products<Super, Fl>(self[1][0], self[2][2], self[1][2], self[2][0]);
        Super minor01 = fl_safe_subtract_products<Super, Fl>(self[1][0], self[2][1], self[1][1], self[2][0]);
    
        return flfma(Super(self[0][2]), minor01,
                fl_subtract_products(Super(self[0][0]), minor12, Super(self[0][1]), minor02));
    }

    template<typename Fl, typename Super>
    Super matrix_determinant(const Fl (&self)[4][4]) {
        Super s0 = fl_safe_subtract_products<Super, Fl>(self[0][0], self[1][1], self[1][0], self[0][1]);
        Super s1 = fl_safe_subtract_products<Super, Fl>(self[0][0], self[1][2], self[1][0], self[0][2]);
        Super s2 = fl_safe_subtract_products<Super, Fl>(self[0][0], self[1][3], self[1][0], self[0][3]);

        Super s3 = fl_safe_subtract_products<Super, Fl>(self[0][1], self[1][2], self[1][1], self[0][2]);
        Super s4 = fl_safe_subtract_products<Super, Fl>(self[0][1], self[1][3], self[1][1], self[0][3]);
        Super s5 = fl_safe_subtract_products<Super, Fl>(self[0][2], self[1][3], self[1][2], self[0][3]);

        Super c0 = fl_safe_subtract_products<Super, Fl>(self[2][0], self[3][1], self[3][0], self[2][1]);
        Super c1 = fl_safe_subtract_products<Super, Fl>(self[2][0], self[3][2], self[3][0], self[2][2]);
        Super c2 = fl_safe_subtract_products<Super, Fl>(self[2][0], self[3][3], self[3][0], self[2][3]);

        Super c3 = fl_safe_subtract_products<Super, Fl>(self[2][1], self[3][2], self[3][1], self[2][2]);
        Super c4 = fl_safe_subtract_products<Super, Fl>(self[2][1], self[3][3], self[3][1], self[2][3]);
        Super c5 = fl_safe_subtract_products<Super, Fl>(self[2][2], self[3][3], self[3][2], self[2][3]);

        return fl_subtract_products(s0, c5, s1, c4)
            + fl_subtract_products(s2, c3, -s3, c2)
            + fl_subtract_products(s5, c0, s4, c1);
    }

    /*********************************************************************************************/
#ifdef __racket__
#define OVERRIDE override

    class __lambda__ MatrixTop {
    public:
        virtual ~MatrixTop() = default;

    public:
        virtual bool is_square_matrix() const noexcept = 0;
        virtual bool is_empty_matrix() const noexcept = 0;
        virtual bool is_row_vector() const noexcept = 0;
        virtual bool is_column_vector() const noexcept = 0;
        virtual bool is_zero_matrix() const noexcept = 0;
        virtual bool is_symmetric_matrix() const noexcept = 0;
        virtual bool is_skew_symmetric_matrix() const noexcept = 0;
        virtual bool is_lower_triangular_matrix() const noexcept = 0;
        virtual bool is_upper_triangular_matrix() const noexcept = 0;
        virtual bool is_diagonal_matrix() const noexcept = 0;
        virtual bool is_identity_matrix() const noexcept = 0;
        virtual bool is_scalar_matrix() const noexcept = 0;
        virtual bool is_fixnum_matrix() const noexcept = 0;
        virtual bool is_flonum_matrix() const noexcept = 0;
        virtual bool is_inexact_fixnum_matrix() const noexcept = 0;

        virtual bool is_row_echelon_form() const noexcept = 0;
        virtual bool is_row_canonical_form() const noexcept = 0;

    public:
        virtual size_t row_size() const noexcept = 0;
        virtual size_t column_size() const noexcept = 0;

    public:
        virtual std::string desc(bool one_line) const noexcept = 0;
    };
#else
#define OVERRIDE
#endif

    /*********************************************************************************************/
    template<size_t M, size_t N = M, typename T = double>
    class __lambda__ Matrix
#ifdef __racket__
        : public GYDM::MatrixTop
#endif
    {
        template<size_t R, size_t C, typename U> friend class GYDM::Matrix;

        using Super = std::enable_if_t<std::is_arithmetic_v<T>, SuperDatum<T>>;

    public:
        Matrix() = default;

        Matrix(T scalar, bool diagonal_only = true) noexcept {
            if (diagonal_only) {
                this->fill_diagonal(scalar);
            } else {
                this->fill(scalar);
            }
        }

        template<size_t R, size_t C, typename U>
        Matrix(const U (&src)[R][C]) noexcept { this->fill(src); }
        
        template<size_t O, typename U>
        Matrix(const U (&src)[O]) noexcept { this->fill(src); }

        template<typename Array1D>
        Matrix(const Array1D& src1D, size_t mn) noexcept { this->fill(src1D, mn); }

        template<typename Array2D>
        Matrix(const Array2D& src2D, size_t rN, size_t cN) noexcept { this->fill(src2D, rN, cN); }

        template<size_t R, size_t C, typename U>
        Matrix(const GYDM::Matrix<R, C, U>& src) noexcept { this->fill(src); }

        template<size_t R, size_t C, typename U>
        Matrix(const GYDM::Matrix<R, C, U>* src) noexcept { this->fill(src); }

    public:
        inline T unsafe_ref(size_t r, size_t c) const noexcept { return this->entries[r][c]; }
        inline void unsafe_set(size_t r, size_t c, T datum) noexcept { this->entries[r][c] = datum; }
        
        T ref(size_t r, size_t c) const { this->check_bounds(r, c); return this->entries[r][c]; }
        void set(size_t r, size_t c, T datum) { this->check_bounds(r, c); this->entries[r][c] = datum; }

        void swap_row(size_t r1, size_t r2) { if (this->rows_okay(r1, r2)) array2d_swap_row(this->entries, N, r1, r2); }
        void swap_column(size_t c1, size_t c2) { if (this->columns_okay(c1, c2)) array2d_swap_column(this->entries, M, c1, c2); }

        template<typename Array1D>
        size_t extract(Array1D& dest1D, size_t size) const noexcept { return array2d_copy_to_array1d(this->entries, M, N, dest1D, size); }

        template<typename Array2D>
        size_t extract(Array2D& dest2D, size_t nR, size_t nC) const noexcept { return array2d_copy_to_array2d(this->entries, M, N, dest2D, nR, nC); }

        template<typename Array1D>
        size_t extract_diagonal(Array1D& dest, size_t size) const noexcept { return array2d_copy_diagonal_to_array1d(this->entries, M, N, dest, size); }

        template<typename Array1D>
        size_t extract_row(size_t r, Array1D& dest, size_t size) const { return array2d_copy_row_to_array1d(this->entries, M, N, dest, size, r); }

        template<typename Array1D>
        size_t extract_column(size_t c, Array1D& dest, size_t size) const { return array2d_copy_column_to_array1d(this->entries, M, N, dest, size, c); }

        template<typename Array2D>
        size_t extract_lower_triangle(Array2D& dest2D, size_t nR, size_t nC) const noexcept
        { return array2d_copy_lower_triangle_to_array2d(this->entries, M, N, dest2D, nR, nC); }

        template<typename Array2D>
        size_t extract_upper_triangle(Array2D& dest2D, size_t nR, size_t nC, bool diagonal = false) const noexcept
        { return array2d_copy_upper_triangle_to_array2d(this->entries, M, N, dest2D, nR, nC, diagonal); }

        void fill(T datum) noexcept { array2d_fill_with_datum(this->entries, M, N, datum); }

        template<typename Array1D>
        void fill(const Array1D& src1D, size_t mn) noexcept { array2d_fill_from_array1d(this->entries, M, N, src1D, mn); }

        template<typename Array2D>
        void fill(const Array2D& src2D, size_t rN, size_t cN) noexcept { array2d_copy_to_array2d(src2D, rN, cN, this->entries, M, N); }

        template<size_t R, size_t C, typename U>
        void fill(const U (&src)[R][C]) noexcept { array2d_copy_to_array2d(src, R, C, this->entries, M, N); }
        
        template<size_t O, typename U, typename = RealDatum<U>>
        void fill(const U (&src)[O]) noexcept { array2d_fill_from_array1d(this->entries, M, N, src, O); }

        template<size_t R, size_t C, typename U>
        void fill(const GYDM::Matrix<R, C, U>& src) noexcept { array2d_copy_to_array2d(src.entries, R, C, this->entries, M, N); }

        template<size_t R, size_t C, typename U>
        void fill(const GYDM::Matrix<R, C, U>* src) noexcept { array2d_copy_to_array2d(src->entries, R, C, this->entries, M, N); }

        void fill_diagonal(T datum) noexcept { array2d_fill_diagonal_with_datum(this->entries, M, N, datum); }

        template<typename Array1D>
        void fill_diagonal(const Array1D& src1D, size_t n) noexcept { array2d_fill_diagonal_from_array1d(this->entries, M, N, src1D, n); }

        template<typename Array2D>
        void fill_diagonal(const Array2D& src2D, size_t rN, size_t cN) noexcept { array2d_fill_diagonal_from_array2d(this->entries, M, N, src2D, rN, cN); }

        template<size_t R, size_t C, typename U>
        void fill_diagonal(const U (&src)[R][C]) noexcept { array2d_copy_diagonal_to_array2d(src, R, C, this->entries, M, N); }
        
        template<size_t O, typename U>
        void fill_diagonal(const U (&src)[O]) noexcept { array2d_fill_diagonal_from_array1d(this->entries, M, N, src, O); }

        template<size_t R, size_t C, typename U>
        void fill_diagonal(const GYDM::Matrix<R, C, U>& src) noexcept { array2d_copy_diagonal_to_array2d(src.entries, R, C, this->entries, M, N); }

        template<size_t R, size_t C, typename U>
        void fill_diagonal(const GYDM::Matrix<R, C, U>* src) noexcept { array2d_copy_diagonal_to_array2d(src->entries, R, C, this->entries, M, N); }

        void fill_lower_triangle(T datum) noexcept { array2d_fill_lower_triangle_with_datum(this->entries, M, N, datum); }

        template<typename Array2D>
        void fill_lower_triangle(const Array2D& src2D, size_t rN, size_t cN) noexcept { array2d_copy_lower_triangle_to_array2d(src2D, rN, cN, this->entries, M, N); }

        template<size_t R, size_t C, typename U>
        void fill_lower_triangle(const U (&src)[R][C]) noexcept { array2d_copy_lower_triangle_to_array2d(src, R, C, this->entries, M, N); }
        
        template<size_t R, size_t C, typename U>
        void fill_lower_triangle(const GYDM::Matrix<R, C, U>& src) noexcept { array2d_copy_lower_triangle_to_array2d(src.entries, R, C, this->entries, M, N); }

        template<size_t R, size_t C, typename U>
        void fill_lower_triangle(const GYDM::Matrix<R, C, U>* src) noexcept { array2d_copy_lower_triangle_to_array2d(src->entries, R, C, this->entries, M, N); }

        void fill_upper_triangle(T datum) noexcept { array2d_fill_upper_triangle_with_datum(this->entries, M, N, datum); }

        template<typename Array2D>
        void fill_upper_triangle(const Array2D& src2D, size_t rN, size_t cN, bool diagonal = false) noexcept
        { array2d_copy_upper_triangle_to_array2d(src2D, rN, cN, this->entries, M, N, diagonal); }
        
        template<size_t R, size_t C, typename U>
        void fill_upper_triangle(const U (&src)[R][C], bool diagonal = false) noexcept
        { array2d_copy_upper_triangle_to_array2d(src, R, C, this->entries, M, N, diagonal); }
        
        template<size_t R, size_t C, typename U>
        void fill_upper_triangle(const GYDM::Matrix<R, C, U>& src, bool diagonal = false) noexcept
        { array2d_copy_upper_triangle_to_array2d(src.entries, R, C, this->entries, M, N, diagonal); }

        template<size_t R, size_t C, typename U>
        void fill_upper_triangle(const GYDM::Matrix<R, C, U>* src, bool diagonal = false) noexcept
        { array2d_copy_upper_triangle_to_array2d(src->entries, R, C, this->entries, M, N, diagonal); }

    public:
	    GYDM::Matrix<M, N, T> operator-() const noexcept { GYDM::Matrix<M, N, T> self; array2d_opposite(self.entries, this->entries, M, N); return self; }
        bool operator!=(const GYDM::Matrix<M, N, T>& m) const noexcept { return !(this->operator==(m)); }
        bool operator==(const GYDM::Matrix<M, N, T>& m) const noexcept { return array2d_equal(this->entries, M, N, m.entries, M, N); }

        template<size_t R, size_t C, typename U>
        bool operator!=(const GYDM::Matrix<R, C, U>& m) const noexcept { return true; }
        
        template<size_t R, size_t C, typename U>
        bool operator==(const GYDM::Matrix<R, C, U>& m) const noexcept { return false; }

        template<size_t R, size_t C, typename U, typename B = bool>
        typename std::enable_if_t<std::is_floating_point_v<T>, B> flequal(const GYDM::Matrix<R, C, U>& m, double epsilon) const noexcept {
            return array2d_equal(this->entries, M, N, m.entries, R, C, epsilon);
        }

        template<size_t R, size_t C, typename U, typename B = bool>
        typename std::enable_if_t<std::is_floating_point_v<T>, B> flequal(const GYDM::Matrix<R, C, U>* m, double epsilon) const noexcept {
            return array2d_equal(this->entries, M, N, m->entries, R, C, epsilon);
        }

        template<typename R>
		GYDM::Matrix<M, N, T>& operator+=(const GYDM::Matrix<M, N, R>& rhs)
        { array2d_add(this->entries, rhs.entries, M, N); return (*this); }
        
        template<typename R>
		GYDM::Matrix<M, N, T>& operator-=(const GYDM::Matrix<M, N, R>& rhs)
        { array2d_subtract(this->entries, rhs.entries, M, N); return (*this); }
		
        GYDM::Matrix<M, N, T>& operator*=(T rhs) { array2d_scalar_multiply(this->entries, rhs, M, N); return (*this); }
        GYDM::Matrix<M, N, T>& operator/=(T rhs) { array2d_divide(this->entries, rhs, M, N); return (*this); }
        
        template<typename L, typename R>
		friend inline GYDM::Matrix<M, N, decltype(L{} + R{})> operator+(const GYDM::Matrix<M, N, L>& lhs, const GYDM::Matrix<M, N, R>& rhs)
        { return GYDM::Matrix<M, N, decltype(L{} + R{})>(lhs, rhs, true); }

        template<typename L, typename R>
		friend inline GYDM::Matrix<M, N, decltype(L{} - R{})> operator-(const GYDM::Matrix<M, N, L>& lhs, const GYDM::Matrix<M, N, R>& rhs)
        { return GYDM::Matrix<M, N, decltype(L{} - R{})>(lhs, rhs, false); }
		
        template<typename L, typename R>
        friend inline GYDM::Matrix<M, N, decltype(L{} * R{})> operator*(const GYDM::Matrix<M, N, L>& lhs, IfRealDatum<R> rhs)
        { return GYDM::Matrix<M, N, decltype(L{} * R{})>(lhs, rhs, true); }
        
        template<typename L, typename R>
        friend inline GYDM::Matrix<M, N, decltype(L{} * R{})> operator*(IfRealDatum<L> lhs, const GYDM::Matrix<M, N, R>& rhs)
        { return GYDM::Matrix<M, N, decltype(L{} * R{})>(rhs, lhs, true); }

        template<typename L, typename R>
        friend inline GYDM::Matrix<M, N, decltype(L{} / R{})> operator/(const GYDM::Matrix<M, N, L> lhs, IfRealDatum<R> rhs)
        { return GYDM::Matrix<M, N, decltype(L{} / R{})>(lhs, rhs, false); }

        template<size_t P, typename L, typename R>
        friend inline GYDM::Matrix<M, P, decltype(L{} * R{})> operator*(const GYDM::Matrix<M, N, L>& lhs, const GYDM::Matrix<N, P, R>& rhs)
        { GYDM::Matrix<M, P, decltype(L{} * R{})> self; array2d_multiply(self.entries, lhs.entries, rhs.entries, M, N, P); return self; }

    public:
        GYDM::Matrix<N, M, T> transpose() const noexcept { GYDM::Matrix<N, M, T> dest; this->transpose(&dest); return dest; }
        GYDM::Matrix<M, N, T> diagonal() const noexcept { GYDM::Matrix<M, N, T> dest; this->diagonal(&dest); return dest; }
        GYDM::Matrix<1, N, T> row(size_t r) const { GYDM::Matrix<1, N, T> dest; this->row(r, &dest); return dest; }
        GYDM::Matrix<M, 1, T> column(size_t c) const { GYDM::Matrix<M, 1, T> dest; this->column(c, &dest); return dest; }
        GYDM::Matrix<M, N, T> lower_triangle() const noexcept { GYDM::Matrix<M, N, T> dest; this->lower_triangle(&dest); return dest; }
        GYDM::Matrix<M, N, T> upper_triangle(bool diagonal = false) const noexcept { GYDM::Matrix<M, N, T> dest; this->upper_triangle(&dest, diagonal); return dest; }
        
        void transpose(GYDM::Matrix<N, M, T>* dest) const noexcept {
            if (this != dest) {
                array2d_transpose(this->entries, dest->entries, M, N);
            }
        }

        void diagonal(GYDM::Matrix<M, N, T>* dest) const noexcept {
            if (this != dest) {
                array2d_copy_diagonal_to_array2d(this->entries, M, N, dest->entries, M, N);
            }
        }

        void row(size_t r, GYDM::Matrix<1, N, T>* dest) const {
            if (this != dest) {
                this->check_bounds(r, 0);
                array2d_copy_row_to_array2d(this->entries, M, N, dest->entries, 1, N, r, 0);
            }
        }

        void column(size_t c, GYDM::Matrix<M, 1, T>* dest) const {
            if (this != dest) {
                this->check_bounds(0, c);
                array2d_copy_column_to_array2d(this->entries, M, N, dest->entries, M, 1, c, 0);
            }
        }

        void lower_triangle(GYDM::Matrix<M, N, T>* dest) const noexcept {
            if (this != dest) {
                dest->fill_lower_triangle(this->entries, M, N);
            }
        }

        void upper_triangle(GYDM::Matrix<M, N, T>* dest, bool diagonal = false) const noexcept {
            if (this != dest) {
                dest->fill_upper_triangle(this->entries, M, N, diagonal);
            }
        }

    public:
        size_t row_size() const noexcept OVERRIDE { return M; }
        size_t column_size() const noexcept OVERRIDE { return N; }

    public:
        bool is_square_matrix() const noexcept OVERRIDE { return (M == N); }
        bool is_empty_matrix() const noexcept OVERRIDE { return (M == 0) && (N == 0); }
        bool is_row_vector() const noexcept OVERRIDE { return (M == 1) && (N > 0); }
        bool is_column_vector() const noexcept OVERRIDE { return (M > 0) && (N == 1); }
        bool is_fixnum_matrix() const noexcept OVERRIDE { return std::is_integral<T>::value; }
        bool is_inexact_fixnum_matrix() const noexcept OVERRIDE { return std::is_floating_point<T>::value && array2d_is_integer(this->entries, M, N); }
        bool is_flonum_matrix() const noexcept OVERRIDE { return std::is_floating_point<T>::value; }
        bool is_zero_matrix() const noexcept OVERRIDE { return array2d_equal(this->entries, M, N, T()); }
        bool is_symmetric_matrix() const noexcept OVERRIDE { return array2d_is_symmetric(this->entries, M, N); }
        bool is_skew_symmetric_matrix() const noexcept OVERRIDE { return array2d_is_skew_symmetric(this->entries, M, N); }
        bool is_lower_triangular_matrix() const noexcept OVERRIDE { return (M == N) && array2d_upper_triangle_equal(this->entries, M, T()); }
        bool is_upper_triangular_matrix() const noexcept OVERRIDE { return (M == N) && array2d_lower_triangle_equal(this->entries, M, T()); }
        bool is_diagonal_matrix() const noexcept OVERRIDE { return (M == N) && array2d_off_diagonal_equal(this->entries, M, T()); }
        bool is_triangular_matrix() const noexcept { return this->is_lower_triangular_matrix() || this->is_upper_triangular_matrix(); }
        bool is_identity_matrix() const noexcept OVERRIDE { return this->is_diagonal_matrix() && array2d_diagonal_equal(this->entries, M, T(1)); }
        bool is_singular_matrix() const noexcept { return (this->determinant() == Super(0)); }
        bool is_row_echelon_form() const noexcept OVERRIDE { return array2d_is_row_echelon_form(this->entries, M, N, T()); }
        bool is_row_canonical_form() const noexcept OVERRIDE { return array2d_is_row_canonical_form(this->entries, M, N, T(), T(1)); }

        bool is_scalar_matrix() const noexcept OVERRIDE {
            return (M > 0)
                && this->is_diagonal_matrix()
                && array2d_diagonal_equal(this->entries, M, this->entries[0][0]);
        }

    public:
        template<typename TT, typename B = bool>
        typename std::enable_if_t<M == N, B> LU_decomposite(GYDM::Matrix<M, N, TT>* L, GYDM::Matrix<M, N, TT>* U) const noexcept {
            return array2d_lu_decomposite(this->entries, L->entries, U->entries);
        }

        template<typename TT, typename B = bool>
        typename std::enable_if_t<M == N, B> LUP_decomposite(GYDM::Matrix<M, N, TT>* L, GYDM::Matrix<M, N, TT>* U, GYDM::Matrix<M, N, TT>* P) const noexcept {
            return array2d_lup_decomposite(this->entries, L->entries, U->entries, P->entries);
        }

    public:
        template<typename E = T>
        typename std::enable_if_t<M == N, E> trace() const noexcept { return array2d_trace(this->entries, N, E(0)); }

        template<typename S = Super>
        typename std::enable_if_t<M == N, S> determinant() const noexcept {
            if constexpr(N > 0) {
                if constexpr(N < 5) {
                    return matrix_determinant<T, S>(this->entries);
                } else {  // Inefficient, but we don't currently use N > 4 anyway...
                    Matrix<N - 1, N - 1, T> submtx;
                    S sign = 1;
                    S det = 0;
                    
                    for (size_t i = 0; i < N; ++ i) {
                        // Submatrix without row 0 and column `i`
                        array2d_reduce(this->entries, N, submtx.entries, 0, i);
                        det += fl_multiply(sign * this->entries[0][i], submtx.determinant());
                        sign = -sign;
                    }
        
                    return det;
                }
            } else { 
                // the empty product as in `x^0 = 1` and `0! = 1`,
                // the 1 is the multiplicative identity.
                return 1;
            }
        }

    public:
        std::string desc(bool one_line = false) const noexcept OVERRIDE
        { return array2d_to_string(this->entries, M, N, 0, one_line); }

    private:
        template<typename Lhs, typename Rhs>
        Matrix(const GYDM::Matrix<M, N, Lhs>& lhs, const GYDM::Matrix<M, N, Rhs>& rhs, bool forward) noexcept {
            if (forward) {
                array2d_add(this->entries, lhs.entries, rhs.entries, M, N);
            } else {
                array2d_subtract(this->entries, lhs.entries, rhs.entries, M, N);
            }
        }

        template<typename Lhs, typename Rhs, typename = RealDatum<Rhs>>
        Matrix(const GYDM::Matrix<M, N, Lhs>& lhs, Rhs rhs, bool forward) noexcept {
            if (forward) {
                array2d_scalar_multiply(this->entries, lhs.entries, rhs, M, N);
            } else {
                array2d_divide(this->entries, lhs.entries, rhs, M, N);
            }
        }

    private:
        inline void check_bounds(size_t r, size_t c) const {
            this->check_row_bound(r);
            this->check_column_bound(c);
        }

        inline bool rows_okay(size_t r1, size_t r2) const {
            this->check_row_bound(r1);
            this->check_row_bound(r2);

            return r1 != r2;
        }

        inline bool columns_okay(size_t c1, size_t c2) const {
            this->check_column_bound(c1);
            this->check_column_bound(c2);

            return c1 != c2;
        }

        inline void check_row_bound(size_t r) const {
            if (r >= M) throw std::out_of_range(make_nstring("row index too large, %zd >= %zd", r, M));
        }

        inline void check_column_bound(size_t c) const {
            if (c >= N) throw std::out_of_range(make_nstring("column index too large, %zd >= %zd", c, N));
        }

    private:
        T entries[M][N] = {};
    };

    /*********************************************************************************************/
    template<size_t N, typename T> using SquareMatrix = GYDM::Matrix<N, N, T>;

    template<size_t M, size_t N> using FlMatrix = GYDM::Matrix<M, N, double>;
    template<size_t M, size_t N> using FxMatrix = GYDM::Matrix<M, N, int>;

    template<size_t N> using FlSquareMatrix = GYDM::SquareMatrix<N, double>;
    template<size_t N> using FxSquareMatrix = GYDM::SquareMatrix<N, int>;
    
    typedef GYDM::FlSquareMatrix<2> FlMatrix2x2;
    typedef GYDM::FlSquareMatrix<3> FlMatrix3x3;
    typedef GYDM::FlSquareMatrix<4> FlMatrix4x4;
    typedef GYDM::FlMatrix<3, 4> FlMatrix3x4;
    typedef GYDM::FlMatrix<4, 3> FlMatrix4x3;

    typedef GYDM::FxSquareMatrix<2> FxMatrix2x2;
    typedef GYDM::FxSquareMatrix<3> FxMatrix3x3;
    typedef GYDM::FxSquareMatrix<4> FxMatrix4x4;
    typedef GYDM::FxMatrix<3, 4> FxMatrix3x4;
    typedef GYDM::FxMatrix<4, 3> FxMatrix4x3;
}
