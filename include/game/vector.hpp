/*
 *  FILENAME:      Vector.hpp
 *
 *  DESCRIPTION:
 *      Vectortor template for N Dimensions including math functions.
 *
 *  NOTES:
 *      Lacks "equal" support for non floating numbers.
 *
 *
 *  AUTHOR:        Leon Schierbach     DATE: 06.10.2018
 *
 *  CHANGES:
 *
 *  TODO:
 *    -add equal operator for non floating numbers
 */

#ifndef Vector_HPP
#define Vector_HPP

#include <array>
#include <cmath>

namespace game
{
  template< bool B, class T = void >
  using enable_if_t = typename std::enable_if<B,T>::type;

  static constexpr float epsilon = 0.0001f;

  template <size_t N, typename Type = float>
  class Vector
  {
    private:
      std::array<Type, N> data;

    public:

      // construct default
      Vector() : data(std::array<Type, N> { 0 }) {};
      // construct parameterlist
      template<typename... NumericArgs, enable_if_t<(sizeof...(NumericArgs) == N)>* = nullptr>
      Vector(NumericArgs... args) : data(std::array<Type, sizeof...(args)> {args...}) {};
      // construct initializerlist
      Vector(const Type (&arr)[N]) { for(auto i = 0u; i < N; i++) { data[i] = arr[i]; } };

      // copy
      Vector(const Vector<N, Type>& cpy) : data(cpy.data) { };
      // copy assign
      Vector& operator=(const Vector<N, Type>& asgn) { data = asgn.data; return *this; };

      // move
      Vector(const Vector<N, Type>&& rhs) : data(std::move(rhs.data)) {};
      // move assign
      Vector& operator=(const Vector<N, Type>&& rhs) { data = std::move(rhs.data); return *this; };

      // destruct
      ~Vector() = default;

      // operators (General)
      Type                operator[](std::size_t id)                                          { if (id >= N) throw std::out_of_range("Vector: out of bounds"); return data.at(id); };
      const Type          operator[](std::size_t id) const                                    { if (id >= N) throw std::out_of_range("Vector: out of bounds"); return data.at(id); };

      // operators (Vectors)
      Vector&             operator+=(const Vector<N, Type>& rhs)                              { for(auto i = 0u; i < N; i++) { data[i] += rhs[i]; } return *this; };
      Vector&             operator-=(const Vector<N, Type>& rhs)                              { for(auto i = 0u; i < N; i++) { data[i] -= rhs[i]; } return *this; };

      friend Vector       operator+ (const Vector<N, Type>& a,   const Vector<N, Type>& b)    { auto result = a; return result += b; }
      friend Vector       operator+ (      Vector<N, Type>& a,   const Vector<N, Type>& b)    { auto result = a; return result += b; }

      friend Vector       operator- (const Vector<N, Type>& a,   const Vector<N, Type>& b)    { auto result = a; return result -= b; }
      friend Type         operator* (const Vector<N, Type>& a,   const Vector<N, Type>& b)    { Type sum = 0; for(auto i = 0u; i < N; i++) { sum += a[i] * b[i]; } return sum; };

      // operators (Factors)
      Vector&             operator*=(const Type val)                                          { for(auto i = 0u; i < N; i++) { data[i] *= val; }    return *this; };
      Vector&             operator/=(const Type val)                                          { for(auto i = 0u; i < N; i++) { data[i] /= val; }    return *this; };

      friend Vector       operator* (const Vector<N, Type>& a,   const Type val)              { auto result = a; return result *= val; }
      friend Vector       operator* (const Type val,             const Vector<N, Type>& a)    { auto result = a; return result *= val; }

      friend Vector       operator/ (const Vector<N, Type>& a,   const Type val)              { auto result = a; return result /= val; }
      friend Vector       operator/ (const Type val,             const Vector<N, Type>& a)    { auto result = a; return result /= val; }

      // operators (Initializer List)
      Vector&             operator=(Type const (&arr)[N])                                     { for(auto i = 0u; i < N; i++) { data[i] = arr[i]; }    return *this; };

      // operators (Bool)
      friend inline bool  operator< (const Vector<N, Type>& lhs, const Vector<N, Type>& rhs)  { Type sumA = 0; Type sumB = 0; for(auto i = 0u; i < N; i++) { sumA += lhs[i] * lhs[i]; sumB += rhs[i] * rhs[i]; } return sumA < sumB; };
      friend inline bool  operator> (const Vector<N, Type>& lhs, const Vector<N, Type>& rhs)  { return rhs < lhs; }
      friend inline bool  operator<=(const Vector<N, Type>& lhs, const Vector<N, Type>& rhs)  { return !(lhs > rhs); }
      friend inline bool  operator>=(const Vector<N, Type>& lhs, const Vector<N, Type>& rhs)  { return !(lhs < rhs); }

      template<typename T = bool>
      friend inline auto  operator==(const Vector<N, Type>& lhs, const Vector<N, Type>& rhs)
        -> typename std::enable_if<std::is_floating_point<Type>::value, T>::type
          { for(auto i = 0u; i < N; i++) { if(!(lhs[i] >= rhs[i] - epsilon && lhs[i] <= rhs[i] + epsilon)) return false; } return true; }
          
      template<typename T = bool>
      friend inline auto  operator==(const Vector<N, Type>& lhs, const Vector<N, Type>& rhs)
        -> typename std::enable_if<!std::is_floating_point<Type>::value, T>::type
          { for(auto i = 0u; i < N; i++) { if(!(lhs[i] == rhs[i] && lhs[i] == rhs[i])) return false; } return true; }
      
      friend inline auto  operator!=(const Vector<N, Type>& lhs, const Vector<N, Type>& rhs)  { return !(lhs == rhs); };
      
      void print()
      {
        printf("Vector<%u>[\n", N);
        for(auto i = 0u; i < N; i++)
        {
          printf("\t%u\n", data[i]);
        }
        printf("]\n");
      };

      // math functions
      float abs() { float sum = 0; for(auto i = 0u; i < N; i++) { sum += data[i] * data[i]; }return std::sqrt(sum); };

      Vector<N, Type> norm() { return Vector<N, Type>( *this / abs() ); };
  };
  
  namespace math
  {
    template<size_t N, typename Type = float>
    float abs(const Vector<N, Type>& vec) { float sum = 0; for(auto i = 0u; i < N; i++) { sum += vec[i] * vec[i]; }return std::sqrt(sum); };

    template<size_t N, typename Type = float>
    float angle(const Vector<N, Type>& a, const Vector<N, Type>& b) { return acos((a * b) / (abs(a + b))); };

    template<size_t N, typename Type = float>
    Vector<N, Type> norm(Vector<N, Type>& vec) { return Vector<N, Type>( vec / vec.abs()); };

    // no ugly sqrt
    template<size_t N, typename Type = float, typename T = bool>
      auto parallel(const Vector<N, Type>& a, const Vector<N, Type>& b)
        -> typename std::enable_if<std::is_floating_point<Type>::value, T>::type
          { float c = a[0] / b[0]; for(auto i = 1u; i < N; i++) { if(!(a[i] / b[i] >= c - epsilon && a[i] / b[i] <= c + epsilon)) return false; } return true; };
          
    template<size_t N, typename Type = float, typename T = bool>
      auto parallel(const Vector<N, Type>& a, const Vector<N, Type>& b)
        -> typename std::enable_if<!std::is_floating_point<Type>::value, T>::type
          { float c = 1.f * a[0] / b[0]; for(auto i = 1u; i < N; i++) { if(!(1.f * a[i] / b[i] >= c - epsilon && 1.f * a[i] / b[i] <= c + epsilon)) return false; } return true; };

    inline Vector<3, float> crossproduct(const Vector<3, float>& a, const Vector<3, float>& b) { return { (a[1]*b[2]) - (a[2]*b[1]),  (a[2]*b[0]) - (a[0]*b[2]), (a[0]*b[1]) - (a[1]*b[0]) }; };
  };
};

#endif /* Vector_HPP */
