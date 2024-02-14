#pragma once

/**
 * Getting the semantics of 2D-Array right is hard, and might not be that of much an effort
 * 
 * Worthy of reading: https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0009r6.html
 **/

#include "../../datum/array.hpp"
#include "../../datum/flonum.hpp"
#include "../../datum/string.hpp"

#include "../mathematics.hpp"

#include <type_traits>

namespace GYDM {
#ifdef OVERRIDE
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

    public:
        virtual size_t row_size() const noexcept = 0;
        virtual size_t column_size() const noexcept = 0;

    public:
        virtual std::string desc(bool one_line) const noexcept = 0;
    };
#endif

    template<size_t M, size_t N = M, typename T = double, typename = RealDatum<T>>
    class __lambda__ Matrix
#ifdef OVERRIDE
        : public GYDM::MatrixTop
#endif
    {
        template<size_t R, size_t C, typename U, typename /* don't redefine the default parameter */>
        friend class GYDM::Matrix;

    public:
        Matrix() = default;

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

        void fill(T datum) noexcept { array2d_fill_with_datum(this->entries, M, N, datum); }

        template<typename Array1D>
        void fill(const Array1D& src1D, size_t mn) noexcept { array2d_fill_from_array1d(this->entries, M, N, src1D, mn); }

        template<typename Array2D>
        void fill(const Array2D& src2D, size_t rN, size_t cN) noexcept { array2d_copy_to_array2d(src2D, rN, cN, this->entries, M, N); }

        template<size_t R, size_t C, typename U, typename = RealDatum<U>>
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

        template<size_t R, size_t C, typename U, typename = RealDatum<U>>
        void fill_diagonal(const U (&src)[R][C]) noexcept { array2d_copy_diagonal_to_array2d(src, R, C, this->entries, M, N); }
        
        template<size_t O, typename U, typename = RealDatum<U>>
        void fill_diagonal(const U (&src)[O]) noexcept { array2d_fill_diagonal_from_array1d(this->entries, M, N, src, O); }

        template<size_t R, size_t C, typename U>
        void fill_diagonal(const GYDM::Matrix<R, C, U>& src) noexcept { array2d_copy_diagonal_to_array2d(src.entries, R, C, this->entries, M, N); }

        template<size_t R, size_t C, typename U>
        void fill_diagonal(const GYDM::Matrix<R, C, U>* src) noexcept { array2d_copy_diagonal_to_array2d(src->entries, R, C, this->entries, M, N); }

        void fill_lower_triangle(T datum) noexcept { array2d_fill_lower_triangle_with_datum(this->entries, M, N, datum); }

        template<typename Array2D>
        void fill_lower_triangle(const Array2D& src2D, size_t rN, size_t cN) noexcept { array2d_copy_lower_triangle_to_array2d(src2D, rN, cN, this->entries, M, N); }

        template<size_t R, size_t C, typename U, typename = RealDatum<U>>
        void fill_lower_triangle(const U (&src)[R][C]) noexcept { array2d_copy_lower_triangle_to_array2d(src, R, C, this->entries, M, N); }
        
        template<size_t R, size_t C, typename U>
        void fill_lower_triangle(const GYDM::Matrix<R, C, U>& src) noexcept { array2d_copy_lower_triangle_to_array2d(src.entries, R, C, this->entries, M, N); }

        template<size_t R, size_t C, typename U>
        void fill_lower_triangle(const GYDM::Matrix<R, C, U>* src) noexcept { array2d_copy_lower_triangle_to_array2d(src->entries, R, C, this->entries, M, N); }

        void fill_upper_triangle(T datum) noexcept { array2d_fill_upper_triangle_with_datum(this->entries, M, N, datum); }

        template<typename Array2D>
        void fill_upper_triangle(const Array2D& src2D, size_t rN, size_t cN) noexcept { array2d_copy_upper_triangle_to_array2d(src2D, rN, cN, this->entries, M, N); }
        
        template<size_t R, size_t C, typename U, typename = RealDatum<U>>
        void fill_upper_triangle(const U (&src)[R][C]) noexcept { array2d_copy_upper_triangle_to_array2d(src, R, C, this->entries, M, N); }
        
        template<size_t R, size_t C, typename U>
        void fill_upper_triangle(const GYDM::Matrix<R, C, U>& src) noexcept { array2d_copy_upper_triangle_to_array2d(src.entries, R, C, this->entries, M, N); }

        template<size_t R, size_t C, typename U>
        void fill_upper_triangle(const GYDM::Matrix<R, C, U>* src) noexcept { array2d_copy_upper_triangle_to_array2d(src->entries, R, C, this->entries, M, N); }

    public:
        bool operator!=(const GYDM::Matrix<M, N, T>& m) const noexcept { return !(this->operator==(m)); }
        bool operator==(const GYDM::Matrix<M, N, T>& m) const noexcept { return array2d_equal(this->entries, M, N, m.entries, M, N); }

        template<size_t R, size_t C, typename U>
        bool operator!=(const GYDM::Matrix<R, C, U>& m) const noexcept { return true; }
        template<size_t R, size_t C, typename U>
        bool operator==(const GYDM::Matrix<R, C, U>& m) const noexcept { return false; }

    public:
        GYDM::Matrix<N, M, T> transpose() const noexcept { GYDM::Matrix<N, M, T> dest; this->transpose(&dest); return dest; }
        void transpose(GYDM::Matrix<N, M, T>* dest) const noexcept {
            if (this != dest) {
                array2d_transpose(this->entries, dest->entries, M, N);
            }
        }

        GYDM::Matrix<M, N, T> diagonal() const noexcept { GYDM::Matrix<M, N, T> dest; this->diagonal(&dest); return dest; }
        void diagonal(GYDM::Matrix<M, N, T>* dest) const noexcept {
            if (this != dest) {
                array2d_copy_diagonal_to_array2d(this->entries, M, N, dest->entries, M, N);
            }
        }

        GYDM::Matrix<1, N, T> row(size_t r) const { GYDM::Matrix<1, N, T> dest; this->row(r, &dest); return dest; }
        void row(size_t r, GYDM::Matrix<1, N, T>* dest) const {
            if (this != dest) {
                this->check_bounds(r, 0);
                array2d_copy_row_to_array2d(this->entries, M, N, dest->entries, 1, N, r, 0);
            }
        }


        GYDM::Matrix<M, 1, T> column(size_t c) const { GYDM::Matrix<M, 1, T> dest; this->column(c, &dest); return dest; }
        void column(size_t c, GYDM::Matrix<M, 1, T>* dest) const {
            if (this != dest) {
                this->check_bounds(0, c);
                array2d_copy_column_to_array2d(this->entries, M, N, dest->entries, M, 1, c, 0);
            }
        }

        GYDM::Matrix<M, N, T> lower_triangle() const noexcept { GYDM::Matrix<M, N, T> dest; this->lower_triangle(&dest); return dest; }
        void lower_triangle(GYDM::Matrix<M, N, T>* dest) const noexcept {
            if (this != dest) {
                dest->fill_lower_triangle(this->entries, M, N);
            }
        }

        GYDM::Matrix<M, N, T> upper_triangle() const noexcept { GYDM::Matrix<M, N, T> dest; this->upper_triangle(&dest); return dest; }
        void upper_triangle(GYDM::Matrix<M, N, T>* dest) const noexcept {
            if (this != dest) {
                dest->fill_upper_triangle(this->entries, M, N);
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
        bool is_flonum_matrix() const noexcept OVERRIDE { return std::is_floating_point<T>::value; }
        bool is_zero_matrix() const noexcept OVERRIDE { return array2d_equal(this->entries, M, N, T()); }
        bool is_symmetric_matrix() const noexcept OVERRIDE { return array2d_is_symmetric(this->entries, M, N); }
        bool is_skew_symmetric_matrix() const noexcept OVERRIDE { return array2d_is_skew_symmetric(this->entries, M, N); }
        bool is_lower_triangular_matrix() const noexcept OVERRIDE { return (M == N) && array2d_upper_triangle_equal(this->entries, M, T()); }
        bool is_upper_triangular_matrix() const noexcept OVERRIDE { return (M == N) && array2d_lower_triangle_equal(this->entries, M, T()); }
        bool is_triangular_matrix() const noexcept { return this->is_lower_triangular_matrix() || this->is_upper_triangular_matrix(); }
        bool is_diagonal_matrix() const noexcept OVERRIDE { return (M == N) && array2d_off_diagonal_equal(this->entries, M, T()); }
        bool is_identity_matrix() const noexcept OVERRIDE { return this->is_diagonal_matrix() && array2d_diagonal_equal(this->entries, M, T(1)); }
        
        bool is_scalar_matrix() const noexcept OVERRIDE {
            return (M > 0)
                && this->is_diagonal_matrix()
                && array2d_diagonal_equal(this->entries, M, this->entries[0][0]);
        }

    public:
        std::string desc(bool one_line = false) const noexcept OVERRIDE { return array2d_to_string(this->entries, M, N, one_line); }

    private:
        inline void check_bounds(size_t r, size_t c) const {
            if (r >= M) throw std::out_of_range(make_nstring("row index too large, %zd >= %zd", r, M));
            if (c >= N) throw std::out_of_range(make_nstring("column index too large, %zd >= %zd", c, N));
        }

    protected:
        T entries[M][N] = {};
    };

    template<size_t N, typename T = double, typename Super = SuperDatum<T>, typename = RealDatum<T>>
    class SquareMatrix : public GYDM::Matrix<N, N, T> {
        template<size_t O, typename U, typename L, typename /* don't redefine the default parameter */>
        friend class GYDM::SquareMatrix;
    public:
        using GYDM::Matrix<N, N, T>::Matrix;

        SquareMatrix() noexcept : GYDM::Matrix<N, N, T>() {}
        SquareMatrix(T diagonal_scalar) noexcept { array2d_fill_diagonal_with_datum(this->entries, N, N, diagonal_scalar); }

    public:
        bool is_singular_matrix() const noexcept { return (this->determinant() == T(0)); }

    public:
        T trace() const noexcept { return array2d_trace(this->entries, N, T(0)); }

        /** WARNING
         * It's quite easy to get incorrect determinant for a large matrix due to overflow,
         * hence the `Super` type declared as a template parameter.
         **/
        Super determinant() const noexcept {
            if constexpr(N > 0) {
                if constexpr(N < 5) {
                    return matrix_determinant<T, Super>(this->entries);
                } else {  // Inefficient, but we don't currently use N > 4 anyway...
                    GYDM::SquareMatrix<N - 1, T, Super> submtx;
                    Super sign = 1;
                    Super det = 0;
                    
                    for (size_t i = 0; i < N; ++ i) {
                        // Submatrix without row 0 and column `i`
                        array2d_reduce(this->entries, N, submtx.entries, 0, i);
                        det += two_product(sign * this->entries[0][i], submtx.determinant());
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
    };

    /*********************************************************************************************/
    template<size_t M, size_t N> using FlMatrix = GYDM::Matrix<M, N, float>;
    template<size_t M, size_t N> using FxMatrix = GYDM::Matrix<M, N, int>;

    template<size_t N> using FlSquareMatrix = GYDM::SquareMatrix<N, float>;
    template<size_t N> using FxSquareMatrix = GYDM::SquareMatrix<N, int>;
    
    typedef GYDM::FlSquareMatrix<2> FlMatrix2x2;
    typedef GYDM::FlSquareMatrix<3> FlMatrix3x3;
    typedef GYDM::FlSquareMatrix<4> FlMatrix4x4;
    typedef GYDM::FlMatrix<4, 3> FlMatrix4x3;

    typedef GYDM::FxSquareMatrix<2> FxMatrix2x2;
    typedef GYDM::FxSquareMatrix<3> FxMatrix3x3;
    typedef GYDM::FxSquareMatrix<4> FxMatrix4x4;
    typedef GYDM::FxMatrix<4, 3> FxMatrix4x3;
}
