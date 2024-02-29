#pragma once

#include <sstream>
#include "flonum.hpp"

/** WARNING
 * Designed for Matrix
 * 
 * Functions generated in this file are not considered safe.
 * You should check the preconditions before using them.
 * 
 * Use at your own risk.
 */

/*************************************************************************************************/
namespace GYDM {
    template<typename T>
    std::string array2d_to_string(const T& self, size_t R, size_t C, size_t w, bool one_line = false) noexcept {
        std::ostringstream s;
        std::string sep = (one_line ? ", [" : "\n [");
        long actual_width = w;

        s << "[[";
    
        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = 0; c < C; ++ c) {
                long self_width = s.tellp();

                s.width(w);
                s << self[r][c];
                actual_width = std::max(long(s.tellp()) - self_width, actual_width);
                s.width(0);
            
                s << ((c < C - 1) ? ", " : "]");
            }

            s << ((r < R - 1) ? sep : "]");
        }

        if (one_line || (actual_width <= w)) {
            return s.str();
        } else {
            return array2d_to_string(self, R, C, actual_width, one_line);
        }
    }

    /*********************************************************************************************/
    template<typename S>
    inline void array1d_permutation_initialize(S& self, size_t N) {
        for (size_t d = 0; d < N; ++ d) {
            self[d] = d;
        }
    }

    template<typename S, typename T>
    size_t array1d_fill_with_datum(S& self, size_t R, size_t C, T datum) noexcept {
        size_t N = R * C;

        for (size_t idx = 0; idx < N; ++ idx) {
            self[idx] = datum;
        }

        return N;
    }

    template<typename S, typename T>
    size_t array1d_fill_from_array1d(S& self, size_t R, size_t C, const T& src, size_t sN) noexcept {
        size_t N = R * C;
        
        N = (N < sN) ? N : sN;
        for (size_t idx = 0; idx < N; ++ idx) {
            self[idx] = src[idx];
        }

        return N;
    }

    template<typename S, typename T>
    size_t array1d_fill_from_array2d(S& self, size_t R, size_t C, const T& src, size_t sR, size_t sC) noexcept {
        R = (R < sR) ? R : sR;
        C = (C < sC) ? C : sC;
        
        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = 0; c < C; ++ c) {
                self[r * C + c] = src[r][c];
            }
        }

        return R * C;
    }
    
    /*********************************************************************************************/
    template<typename S, typename T>
    size_t array2d_fill_with_datum(S& self, size_t R, size_t C, T datum) noexcept {
        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = 0; c < C; ++ c) {
                self[r][c] = datum;
            }
        }

        return R * C;
    }

    template<typename S>
    size_t array2d_fill_based_on_row_permutation(S& self, size_t R, size_t C, const size_t pi[], size_t N) noexcept {
        for (size_t r = 0; r < R; ++ r) {
            if (r >= N) return r * C;

            for (size_t c = 0; c < C; ++ c) {
                self[r][c] = (c == pi[r]) ? 1 : 0;
            }
        }

        return R * C;
    }

    template<typename S, typename T>
    size_t array2d_fill_from_array1d(S& self, size_t R, size_t C, const T& src, size_t sN) noexcept {
        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = 0; c < C; ++ c) {
                size_t idx = r * C + c;

                if (idx >= sN) return idx;
                self[r][c] = src[idx];
            }
        }

        return R * C;
    }

    template<typename S, typename T>
    size_t array2d_fill_from_array1d(S& self, size_t R, size_t C, const T& src, size_t sR, size_t sC) noexcept {
        R = (R < sR) ? R : sR;
        C = (C < sC) ? C : sC;
        
        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = 0; c < C; ++ c) {
                self[r][c] = src[r * sC + c];
            }
        }

        return R * C;
    }

    template<typename S, typename T>
    size_t array2d_fill_row_with_datum(S& self, size_t R, size_t C, T datum, size_t r) noexcept {
        if (r >= R) return 0;

        for (size_t c = 0; c < C; ++ c) {
            self[r][c] = datum;
        }

        return C;
    }

    template<typename S, typename T>
    size_t array2d_fill_row_from_array1d(S& self, size_t R, size_t C, const T& src, size_t sN, size_t r) noexcept {
        if (r >= R) return 0;

        C = (C < sN) ? C : sN;
        for (size_t c = 0; c < C; ++ c) {
            self[r][c] = src[c];
        }

        return C;
    }

    template<typename S, typename T>
    size_t array2d_fill_column_with_datum(S& self, size_t R, size_t C, T datum, size_t c) noexcept {
        if (c >= C) return 0;

        for (size_t r = 0; r < R; ++ r) {
            self[r][c] = datum;
        }

        return R;
    }

    template<typename S, typename T>
    size_t array2d_fill_column_from_array1d(S& self, size_t R, size_t C, const T& src, size_t sN, size_t c) noexcept {
        if (c >= C) return 0;

        R = (R < sN) ? R : sN;
        for (size_t r = 0; r < R; ++ r) {
            self[r][c] = src[r];
        }

        return C;
    }

    template<typename S, typename T>
    size_t array2d_fill_diagonal_with_datum(S& self, size_t R, size_t C, T datum) noexcept {
        size_t D = (R < C) ? R : C;

        for (size_t d = 0; d < D; ++ d) {
            self[d][d] = datum;
        }

        return D;
    }

    template<typename S, typename T>
    size_t array2d_fill_diagonal_from_array1d(S& self, size_t R, size_t C, const T& src, size_t sN) noexcept {
        size_t D = (R < C) ? R : C;

        D = (D < sN) ? D : sN;
        for (size_t d = 0; d < D; ++ d) {
            self[d][d] = src[d];
        }

        return D;
    }

    template<typename S, typename T>
    size_t array2d_fill_lower_triangle_with_datum(S& self, size_t R, size_t C, T datum) noexcept {
        size_t N = 0;

        for (size_t r = 1; r < R; ++ r) {
            size_t d = (r < C) ? r : C;

            N += d;
            for (size_t c = 0; c < d; ++ c) {
                self[r][c] = datum;
            }
        }

        return N;
    }

    template<typename S, typename T>
    size_t array2d_fill_upper_triangle_with_datum(S& self, size_t R, size_t C, T datum) noexcept {
        size_t N = 0;

        for (size_t r = 0; r < R; ++ r) {
            N += (C - r - 1);

            for (size_t c = r + 1; c < C; ++ c) {
                self[r][c] = datum;
            }
        }

        return N;
    }

    /*********************************************************************************************/
    template<typename S, typename T>
    size_t array2d_copy_to_array1d(const S& src, size_t sR, size_t sC, T& dest, size_t dN) noexcept {
        for (size_t r = 0; r < sR; ++ r) {
            for (size_t c = 0; c < sC; ++ c) {
                size_t idx = r * sC + c;

                if (idx >= dN) return idx;
                dest[idx] = src[r][c];
            }
        }

        return sR * sC;
    }

    template<typename S, typename T>
    size_t array2d_copy_to_array2d(const S& src, size_t sR, size_t sC, T& dest, size_t dR, size_t dC) noexcept {
        size_t R = (sR < dR) ? sR : dR;
        size_t C = (sC < dC) ? sC : dC;

        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = 0; c < C; ++ c) {
                dest[r][c] = src[r][c];
            }
        }

        return R * C;
    }

    template<typename S, typename T>
    size_t array2d_copy_row_to_array1d(const S& src, size_t sR, size_t sC, T& dest, size_t dN, size_t r) noexcept {
        if (r >= sR) return 0;
        
        dN = (dN < sC) ? dN : sC;
        for (size_t c = 0; c < dN; ++ c) {
            dest[c] = src[r][c];
        }

        return dN;
    }

    template<typename S, typename T>
    size_t array2d_copy_row_to_array2d(const S& src, size_t sR, size_t sC, T& dest, size_t dR, size_t dC, size_t sr, size_t dr) noexcept {
        if (sr >= sR) return 0;
        if (dr >= dR) return 0;

        dC = (dC < sC) ? dC : sC;
        for (size_t c = 0; c < dC; ++ c) {
            dest[dr][c] = src[sr][c];
        }

        return dC;
    }

    template<typename S, typename T>
    inline size_t array2d_copy_row_to_array2d(const S& src, size_t sR, size_t sC, T& dest, size_t dR, size_t dC, size_t r) noexcept {
        return array2d_copy_row_to_array2d(src, sR, sC, dest, dR, dC, r, r);
    }

    template<typename S, typename T>
    size_t array2d_copy_column_to_array1d(const S& src, size_t sR, size_t sC, T& dest, size_t dN, size_t c) noexcept {
        if (c >= sC) return 0;

        dN = (dN < sR) ? dN : sR;
        for (size_t r = 0; r < dN; ++ r) {
            dest[r] = src[r][c];
        }

        return dN;
    }

    template<typename S, typename T>
    size_t array2d_copy_column_to_array2d(const S& src, size_t sR, size_t sC, T& dest, size_t dR, size_t dC, size_t sc, size_t dc) noexcept {
        if (sc >= sC) return 0;
        if (dc >= dC) return 0;
        
        dR = (dR < sR) ? dR : sR;
        for (size_t r = 0; r < dR; ++ r) {
            dest[r][dc] = src[r][sc];
        }

        return dR;
    }

    template<typename S, typename T>
    inline size_t array2d_copy_column_to_array2d(const S& src, size_t sR, size_t sC, T& dest, size_t dR, size_t dC, size_t c) noexcept {
        return array2d_copy_column_to_array2d(src, sR, sC, dest, dR, dC, c, c);
    }

    template<typename S, typename T>
    size_t array2d_copy_diagonal_to_array1d(const S& src, size_t sR, size_t sC, T& dest, size_t dN) noexcept {
        size_t D = (sR < sC) ? sR : sC;

        D = (D < dN) ? D : dN;
        for (size_t d = 0; d < D; ++ d) {
            dest[d] = src[d][d];
        }

        return D;
    }

    template<typename S, typename T>
    size_t array2d_copy_diagonal_to_array2d(const S& src, size_t sR, size_t sC, T& dest, size_t dR, size_t dC) noexcept {
        size_t R = (sR < dR) ? sR : dR;
        size_t C = (sC < dC) ? sC : dC;
        size_t D = (R < C) ? R : C;

        for (size_t d = 0; d < D; ++ d) {
            dest[d][d] = src[d][d];
        }

        return D;
    }

    template<typename S, typename T>
    size_t array2d_copy_lower_triangle_to_array2d(const S& src, size_t sR, size_t sC, T& dest, size_t dR, size_t dC) noexcept {
        size_t R = (sR < dR) ? sR : dR;
        size_t C = (sC < dC) ? sC : dC;
        size_t N = 0;

        for (size_t r = 1; r < R; ++ r) {
            size_t d = (r < C) ? r : C;
            
            N += d;
            for (size_t c = 0; c < d; ++ c) {
                dest[r][c] = src[r][c];
            }
        }

        return N;
    }

    template<typename S, typename T>
    size_t array2d_copy_upper_triangle_to_array2d(const S& src, size_t sR, size_t sC, T& dest, size_t dR, size_t dC, bool diagonal) noexcept {
        size_t R = (sR < dR) ? sR : dR;
        size_t C = (sC < dC) ? sC : dC;
        size_t N = 0;
        size_t offset = (diagonal ? 0 : 1);

        for (size_t r = 0; r < R; ++ r) {
            N += (C - r - 1);

            for (size_t c = r + offset; c < C; ++ c) {
                dest[r][c] = src[r][c];
            }
        }

        return N;
    }

    /*********************************************************************************************/
    template<typename S, typename T>
    bool array2d_equal(const S& self, size_t R, size_t C, T datum) noexcept {
        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = 0; c < C; ++ c) {
                if (self[r][c] != datum) return false;
            }
        }

        return true;
    }

    template<typename T>
    bool array2d_equal(const T& self, size_t sR, size_t sC, const T& target, size_t tR, size_t tC) noexcept {
        if (sR != tR) return false;
        if (sC != tC) return false;

        for (size_t r = 0; r < sR; ++ r) {
            for (size_t c = 0; c < sC; ++ c) {
                if (self[r][c] != target[r][c]) return false;
            }
        }

        return true;
    }

    template<typename T, typename E>
    bool array2d_equal(const T& self, size_t sR, size_t sC, const T& target, size_t tR, size_t tC, E epsilon) noexcept {
        if (sR != tR) return false;
        if (sC != tC) return false;

        for (size_t r = 0; r < sR; ++ r) {
            for (size_t c = 0; c < sC; ++ c) {
                if (!flequal(self[r][c], target[r][c], epsilon)) return false;
            }
        }

        return true;
    }

    template<typename S, typename T>
    bool array2d_diagonal_equal(const S& self, size_t D, T datum) noexcept {
        for (size_t d = 0; d < D; ++ d) {
            if (self[d][d] != datum) return false;
        }

        return true;
    }

    template<typename T>
    bool array2d_diagonal_equal(const T& self, size_t sD, const T& target, size_t tD) noexcept {
        if (sD != tD) return false;

        for (size_t d = 0; d < sD; ++ d) {
            if (self[d][d] != target[d][d]) return false;
        }

        return true;
    }

    template<typename S, typename T>
    bool array2d_off_diagonal_equal(const S& self, size_t D, T datum) noexcept {
        for (size_t r = 1; r < D; ++ r) {
            for (size_t c = 0; c < r; ++ c) {
                if (self[r][c] != datum) return false;
                if (self[c][r] != datum) return false;
            }
        }

        return true;
    }

    template<typename T>
    bool array2d_off_diagonal_equal(const T& self, size_t sD, const T& target, size_t tD) noexcept {
        if (sD != tD) return false;

        for (size_t r = 1; r < sD; ++ r) {
            for (size_t c = 0; c < r; ++ c) {
                if (self[r][c] != target[r][c]) return false;
                if (self[c][r] != target[c][r]) return false;
            }
        }

        return true;
    }

    template<typename S, typename T>
    bool array2d_off_diagonal_equal(const S& self, size_t R, size_t C, T datum) noexcept {
        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = 0; c < C; ++ c) {
                if (r != c) {
                    if (self[r][c] != datum) return false;
                }
            }
        }

        return true;
    }

    template<typename T>
    bool array2d_off_diagonal_equal(const T& self, size_t sR, size_t sC, const T& target, size_t tR, size_t tC) noexcept {
        if (sR != tR) return false;
        if (sC != tC) return false;

        for (size_t r = 0; r < sR; ++ r) {
            for (size_t c = 0; c < sC; ++ c) {
                if (r != c) {
                    if (self[r][c] != target[r][c]) return false;
                }
            }
        }

        return true;
    }
    
    template<typename S, typename T>
    bool array2d_lower_triangle_equal(const S& self, size_t D, T datum) noexcept {
        for (size_t r = 1; r < D; ++ r) {
            for (size_t c = 0; c < r; ++ c) {
                if (self[r][c] != datum) return false;
            }
        }

        return true;
    }

    template<typename S, typename T>
    bool array2d_upper_triangle_equal(const S& self, size_t D, T datum) noexcept {
        for (size_t r = 0; r < D; ++ r) {
            for (size_t c = r + 1; c < D; ++ c) {
                if (self[r][c] != datum) return false;
            }
        }

        return true;
    }

    template<typename S, typename T>
    bool array2d_lower_triangle_equal(const S& self, size_t R, size_t C, T datum) noexcept {
        for (size_t r = 1; r < R; ++ r) {
            size_t d = (r < C) ? r : C;

            for (size_t c = 0; c < d; ++ c) {
                if (self[r][c] != datum) return false;
            }
        }

        return true;
    }

    template<typename S, typename T>
    bool array2d_upper_triangle_equal(const S& self, size_t R, size_t C, T datum) noexcept {
        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = r + 1; c < C; ++ c) {
                if (self[r][c] != datum) return false;
            }
        }

        return true;
    }

    template<typename T>
    inline bool array2d_diagonal_equal(const T& self, size_t sR, size_t sC, const T& target, size_t tR, size_t tC) noexcept {
        return array2d_diagonal_equal(self, (sR < sC) ? sR : sC, target, (tR < tC) ? tR : tC);
    }

    template<typename S, typename T>
    inline bool array2d_diagonal_equal(const S& self, size_t R, size_t C, T datum) noexcept {
        return array2d_diagonal_equal(self, (R < C) ? R : C, datum);
    }

    /*********************************************************************************************/
    template<typename S>
    bool array2d_is_integer(const S& self, size_t R, size_t C) noexcept {
        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = 0; c < C; ++ c) {
                if (!flisinteger(self[r][c])) {
                    return false;
                }
            }
        }

        return true;
    }

    template<typename S>
    bool array2d_is_symmetric(const S& self, size_t R, size_t C) noexcept {
        if (R != C) return false;

        for (size_t r = 1; r < R; ++ r) {
            for (size_t c = 0; c < r; ++ c) {
                if (self[r][c] != self[c][r]) {
                    return false;
                }
            }
        }

        return true;
    }

    template<typename S>
    bool array2d_is_skew_symmetric(const S& self, size_t R, size_t C) noexcept {
        if (R != C) return false;

        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = 0; c <= r; ++ c) {
                if (self[r][c] != -self[c][r]) {
                    return false;
                }
            }
        }

        return true;
    }

    template<typename S, typename T>
    bool array2d_is_row_echelon_form(const S& self, size_t R, size_t C, T zdatum) noexcept {
        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = r; c < C; ++ c) {
                if (self[r][c] != zdatum) {
                    for (size_t b = r + 1; b < R; ++ b) {
                        if (self[b][c] != zdatum) {
                            return false;
                        }
                    }

                    break;
                }
            }
        }

        return true;
    }

    template<typename S, typename T>
    bool array2d_is_row_canonical_form(const S& self, size_t R, size_t C, T zdatum, T odatum) noexcept {
        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = r; c < C; ++ c) {
                if (self[r][c] != zdatum) {
                    if (self[r][c] == odatum) {
                        for (size_t a = 0; a < r; ++ a) {
                            if (self[a][c] != zdatum) {
                                return false;
                            }
                        }
                        
                        for (size_t b = r + 1; b < R; ++ b) {
                            if (self[b][c] != zdatum) {
                                return false;
                            }
                        }
                    
                        break;
                    } else {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    /*********************************************************************************************/
    template<typename S>
    void array2d_opposite(S& self, const S& s, size_t R, size_t C) noexcept {
        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = 0; c < C; ++ c) {
                self[r][c] = -s[r][c];
            }
        }
    }

    template<typename S, typename Lhs, typename Rhs>
    void array2d_add(S& self, const Lhs& lhs, const Rhs& rhs, size_t R, size_t C) noexcept {
        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = 0; c < C; ++ c) {
                self[r][c] = lhs[r][c] + rhs[r][c];
            }
        }
    }
    
    template<typename Lhs, typename Rhs>
    void array2d_add(Lhs& self, const Rhs& rhs, size_t R, size_t C) noexcept {
        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = 0; c < C; ++ c) {
                self[r][c] += rhs[r][c];
            }
        }
    }

    template<typename S, typename Lhs, typename Rhs>
    void array2d_subtract(S& self, const Lhs& lhs, const Rhs& rhs, size_t R, size_t C) noexcept {
        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = 0; c < C; ++ c) {
                self[r][c] = lhs[r][c] - rhs[r][c];
            }
        }
    }

    template<typename Lhs, typename Rhs>
    void array2d_subtract(Lhs& self, const Rhs& rhs, size_t R, size_t C) noexcept {
        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = 0; c < C; ++ c) {
                self[r][c] -= rhs[r][c];
            }
        }
    }

    template<typename S, typename Lhs, typename Rhs>
    void array2d_scalar_multiply(S& self, const Lhs& lhs, Rhs rhs, size_t R, size_t C) noexcept {
        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = 0; c < C; ++ c) {
                self[r][c] = lhs[r][c] * rhs;
            }
        }
    }

    template<typename S, typename Rhs>
    void array2d_scalar_multiply(S& self, Rhs rhs, size_t R, size_t C) noexcept {
        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = 0; c < C; ++ c) {
                self[r][c] *= rhs;
            }
        }
    }

    template<typename S, typename Lhs, typename Rhs>
    void array2d_divide(S& self, const Lhs& lhs, Rhs rhs, size_t R, size_t C) noexcept {
        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = 0; c < C; ++ c) {
                self[r][c] = lhs[r][c] / rhs;
            }
        }
    }

    template<typename S, typename Rhs>
    void array2d_divide(S& self, Rhs rhs, size_t R, size_t C) noexcept {
        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = 0; c < C; ++ c) {
                self[r][c] /= rhs;
            }
        }
    }

    template<typename S, typename Lhs, typename Rhs>
    void array2d_multiply(S& self, const Lhs& lhs, const Rhs& rhs, size_t M, size_t N, size_t P) noexcept {
        for (size_t r = 0; r < M; ++ r) {
            for (size_t c = 0; c < P; ++ c) {
                self[r][c] = 0;

                for (size_t n = 0; n < N; ++ n) {
                    self[r][c] += lhs[r][n] * rhs[n][c];
                }
            }
        }
    }

    template<typename S, typename Rhs>
    void array2d_multiply(S& self, Rhs rhs, size_t N) noexcept {
        for (size_t r = 0; r < N; ++ r) {
            for (size_t c = 0; c < N; ++ c) {
                self[r][c] *= rhs;
            }
        }
    }

    /*********************************************************************************************/
    template<typename S>
    inline void array2d_swap_row(S& self, size_t C, size_t r1, size_t r2) noexcept {
        using std::swap;
        swap(self[r1], self[r2]);
    }

    template<typename S>
    inline void array2d_swap_column(S& self, size_t R, size_t c1, size_t c2) noexcept {
        using std::swap;
        for (size_t r = 0; r < R; ++ r) {
            swap(self[r][c1], self[r][c2]);
        }
    }
    
    template<typename S, typename T>
    void array2d_transpose(const S& self, const T& dest, size_t R, size_t C) noexcept {
        for (size_t r = 0; r < R; ++ r) {
            for (size_t c = 0; c < C; ++ c) {
                dest[c][r] = self[r][c];
            }
        }
    }

    template<typename S, typename T>
    T array2d_trace(const S& self, size_t order, T datum0) noexcept {
        T sum = datum0;

        for (size_t d = 0; d < order; ++ d) {
            sum += self[d][d];
        }

        return sum;
    }

    template<typename S, typename T>
    void array2d_reduce(const S& self, size_t order, T& dest, size_t ex_row, size_t ex_col) noexcept {
        size_t D_1 = order - 1;

        for (size_t r = 0; r < D_1; ++ r) {
            for (size_t c = 0; c < D_1; ++ c) {
                size_t sR = (r < ex_row) ? r : r + 1;
                size_t sC = (c < ex_col) ? c : c + 1;

                dest[r][c] = self[sR][sC];
            }
        }
    }

    template<size_t N, typename S, typename T>
    bool array2d_lu_decomposite(const S (&self)[N][N], T (&L)[N][N], T (&U)[N][N]) noexcept {
        /**
         * O(n^3)  
         * M = | a01  wT | = |     1  0 | | a01            wT |
         *     |   v  A' |   | v/a01  I | |   0  A' - vwT/a01 |
         **/
        
        // the upper triangular matrix also holds the `A' - vwT/a11` recursively,
        // thus, its lower portion must be initialized as well.
        array2d_copy_to_array2d(self, N, N, U, N, N);
        
        for (size_t k = 0; k < N; ++ k) {
            // Symmetric positive-definite matrix doesn't have such a problem
            if (U[k][k] == 0) return false;

            L[k][k] = 1; // lower triangle has all 1s diagonal
            for (size_t r = k + 1; r < N; ++ r) {
                // fill the Lower triangle, v
                L[r][k] = U[r][k] / U[k][k];

                // compute the elements of the Schur complement `A' - vwT/a11`,
                // as well as scale follow-up `wT`s in place
                for (size_t c = k + 1; c < N; ++ c) {
                    U[r][c] -= L[r][k] * U[k][c];
                }
            }
        }

        // clear the lower portion from the upper triangular matrix
        array2d_fill_lower_triangle_with_datum(U, N, N, T(0));

        return true;
    }

    template<size_t N, typename S, typename T>
    bool array2d_lup_decomposite(const S (&self)[N][N], T (&L)[N][N], T (&U)[N][N], size_t (&P)[1][N]) noexcept {
        using std::swap;

        /**
         * O(n^3)  
         * PM = | ak1  wT | = |     1  0 | | ak1            wT |
         *      |   v  A' |   | v/ak1  I | |   0  A' - vwT/ak1 |
         **/
        
        // the upper triangular matrix also holds the `A' - vwT/a11` recursively,
        // thus, its lower portion must be initialized as well.
        array2d_copy_to_array2d(self, N, N, U, N, N);
        array1d_permutation_initialize(P[0], N);
        
        for (size_t k = 0; k < N; ++ k) {
            T pivot = 0;
            size_t tk;

            // search based on the largest absolute value can somehow
            //   avoid dividing by floating numbers very closing to 0 
            for (size_t r = k; r < N; ++ r) {
                if (U[r][k] > pivot) {
                    pivot = U[r][k];
                    tk = r;
                } else if (U[r][k] < -pivot) {
                    pivot = -U[r][k];
                    tk = r;
                }
            }

            // singular(non-invertible) matrix
            if (pivot == 0) return false;

            if (k != tk) {
                swap(P[0][k], P[0][tk]);
                array2d_swap_row(U, N, k, tk);
            }

            L[k][k] = 1; // lower triangle has all 1s diagonal
            for (size_t r = k + 1; r < N; ++ r) {
                // fill the Lower triangle, v
                U[r][k] = U[r][k] / U[k][k];

                // compute the elements of the Schur complement `A' - vwT/ak1`,
                // as well as scale follow-up `wT`s in place
                for (size_t c = k + 1; c < N; ++ c) {
                    U[r][c] -= U[r][k] * U[k][c];
                }
            }
        }

        // clear the lower portion from the upper triangular matrix
        array2d_copy_lower_triangle_to_array2d(U, N, N, L, N, N);
        array2d_fill_lower_triangle_with_datum(U, N, N, T(0));

        return true;
    }
}
