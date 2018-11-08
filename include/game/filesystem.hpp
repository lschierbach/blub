/* 
 *  FILENAME:      filesystem.hpp
 * 
 *  DESCRIPTION:
 *      offers template functions for saving/loading structs or vectors of structs
 *
 *  PUBLIC FUNCTIONS:
 *      struct bool           is_range TRUE if Type has methods: size, begin, end
 *      struct bool           is_saveable TRUE if Type has methods: read, write
 *
 *      void        template  writeStruct(std::ofstream&, Type) -> isPrimitive
 *      void        template  writeStruct(std::ofstream&, Type) -> !isPrimitive
 *      void        template  writeStruct(const std::string&, Type)
 *
 *      void        template  writeContainer(std::ofstream&, Type) -> isrange
 *      void        template  writeContainer(std::ofstream&, Type) -> !isrange
 *      void        template  writeContainer(const std::string&, Type)
 *
 *      void        template  readStruct(std::ifstream&, Type) -> isPrimitive
 *      void        template  readStruct(std::ifstream&, Type) -> !isPrimitive
 *      void        template  readStruct(const std::string&, Type)
 *
 *      void        template  readContainer(std::ifstream&, Type) -> isRange
 *      void        template  readContainer(std::ifstream&, Type) -> !isRange
 *      void        template  readContainer(const std::string&, Type) 
 *      
 *      bool        fileExists(const std::string)
 *      
 *  NOTES:
 *      When trying to save a struct or a vector of structs with non primitive data (string, vector, ..) it should inherit from class "Saveable" or provide methods "save" and write".
 *      
 *  AUTHOR:        Leon Schierbach     DATE: 18.09.2018
 *
 *  CHANGES: 
 *      LS, 20.09.2018
 *                 -Added methods for handling non primitive data 
 *                 -Added compatibility for structs inherited from Saveable
 *      
 *      LS, 21.10.2018
 *                 -Handles files now in Binary to ignore control-characters 
 *
 *      LS, 25.10.2018
 *                 -Saves/Loads non-primitive Structs using streaming operators << & >> instead as Byte-Array
 */
#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <vector>
#include <fstream>
#include <ostream>
#include <variant>

template<typename... Ts> struct make_void { typedef void type;};
template<typename... Ts> using void_t = typename make_void<Ts...>::type;

template <typename T, typename = void>
struct has_size : public std::false_type {};

template <typename T>
struct has_size<T, void_t<decltype(std::declval<T>().size())>> : public std::true_type {};

template <typename T, typename = void>
struct is_iterable : public std::false_type {};

template <typename T>
struct is_iterable<T, void_t<decltype(std::declval<T>().begin()), decltype(std::declval<T>().end())>> : public std::true_type {};

template <typename T, typename = void>
struct has_index : public std::false_type {};

template <typename T>
struct has_index<T, void_t<decltype(std::declval<T>().index())>> :  public std::true_type {};





template <typename Type>
struct is_range : public std::integral_constant<bool, (has_size<Type>::value && is_iterable<Type>::value)> {};

template <typename T>
struct is_variant : public std::integral_constant<bool, (has_index<T>::value)> {};

template< bool B, class T = void >
using enable_if_t = typename std::enable_if<B,T>::type;

namespace filesystem
{
  
  ////////////////////// WRITE_STRUCT /////////////////////
  
  template<typename Struct>
  auto writeStruct(std::ofstream& out, Struct& saveableStruct)  -> enable_if_t<!std::is_fundamental<Struct>::value && !is_variant<Struct>::value>
  {
    out << saveableStruct;
  }

  template<typename Fundamental>
  auto writeStruct(std::ofstream& out, Fundamental& primitive)  -> enable_if_t<std::is_fundamental<Fundamental>::value && !is_variant<Fundamental>::value>
  {
    out.write(reinterpret_cast<const char*>(&primitive), sizeof(Fundamental));
  }

  template<typename Struct>
  void writeStruct(const std::string& filePath, Struct& strct) 
  {
    std::ofstream fstream(filePath, std::ios::out | std::ios::binary);
    writeStruct(fstream, strct);
  }
  
  ////////////////////// WRITE_VARIANT /////////////////////

  template <typename Variant>
  auto writeStruct(std::ofstream& out, Variant& variant)  -> enable_if_t<is_variant<Variant>::value>
  {
    uint32_t index = variant.index();
    writeStruct<uint32_t>(out, index);
    
    std::visit
    ([&](auto&& arg) -> void
      {
        writeStruct(out, arg);
      }, variant
    );
  }
  
  ////////////////////// WRITE_RANGE //////////////////////
  
  template <typename Struct>
  auto writeRange(std::ofstream& out, Struct& strct)            -> enable_if_t<!is_range<Struct>::value>
  {
    writeStruct(out, strct);
  }

  template<typename Range>
  auto writeRange(std::ofstream& out, Range& currentRange)      -> enable_if_t<is_range<Range>::value>
  {
    uint32_t size = currentRange.size();
    writeStruct<uint32_t>(out, size);
    
    for(auto& elem : currentRange)
    {
      writeRange(out, elem);
    }
  }
  
  template <typename Range>
  void writeRange(const std::string& filePath, Range& range)
  {
    std::ofstream fstream(filePath, std::ios::out | std::ios::binary);
    writeRange(fstream, range);
  }
  
  ////////////////////// READ_STRUCT //////////////////////
  
  
  template <typename Variant, typename Indices = std::make_index_sequence<std::variant_size_v<Variant>>>
  auto readVariant(std::ifstream& in, Variant& variant) -> void;
  
  template<typename Variant>
  auto readStruct(std::ifstream& in, Variant& variant)    -> enable_if_t<(is_variant<Variant>::value)>
  {
    readVariant(in, variant);
  }
  
  template<typename Struct>
  auto readStruct(std::ifstream& in, Struct& saveableStruct)    -> enable_if_t<(!std::is_fundamental<Struct>::value && !is_variant<Struct>::value)>
  {
    in >> saveableStruct;
  }

  template<typename Fundamental>
  auto readStruct(std::ifstream& in, Fundamental& primitive)    -> enable_if_t<(std::is_fundamental<Fundamental>::value && !is_variant<Fundamental>::value)>
  {
    in.read(reinterpret_cast<char*>(&primitive), sizeof(Fundamental));
  }

  template<typename Struct>
  void readStruct(const std::string& filePath, Struct& strct) 
  {
    std::ifstream fstream(filePath, std::ios::in | std::ios::binary);
    readStruct(fstream, strct);
  }
  
  ////////////////////// READ_VARIANT //////////////////////
  
  template <std::size_t I, typename Variant>
  auto loadVariantType(std::ifstream& in, Variant& variant, size_t index)
  {
      if (index == I)
      {
          using T = std::decay_t<decltype(std::get<I>(variant))>;
          T result;
          readStruct(in, result);
          variant = result;
      }
  }

  template <typename Variant, std::size_t... I>
  auto loadVariantImpl(std::ifstream& in, size_t index, Variant& variant, std::index_sequence<I...>) -> void
  {
      (loadVariantType<I>(in, variant, index), ...);
  }

  template <typename Variant, typename Indices = std::make_index_sequence<std::variant_size_v<Variant>>>
  auto readVariant(std::ifstream& in, Variant& variant) -> void// std::enable_if_t<is_variant<Variant>::value, void>
  {
      uint32_t index;
      readStruct<uint32_t>(in, index);
      loadVariantImpl(in, index, variant, Indices { });
  }

  /////////////////////// READ_RANGE //////////////////////
  
  template<typename Struct>
  auto readRange(std::ifstream& in, Struct& strct)              -> enable_if_t<!is_range<Struct>::value>
  {
    readStruct(in, strct);
  }

  template<typename Range>
  auto readRange(std::ifstream& in, Range& range)               -> enable_if_t<is_range<Range>::value>
  {
    uint32_t size;
    readStruct<uint32_t>(in, size);
    
    for (auto i = 0; i < size; i++)
    {
      typename Range::value_type elem;
      readRange(in, elem);
      range.push_back(elem);
    }
  }

  template<typename Range>
  void readRange(const std::string& filePath, Range& range)
  {
    std::ifstream filestream(filePath, std::ios::in | std::ios::binary);
    readRange(filestream, range);
  }
  
  ////////////////////// NON_TEMPLATE /////////////////////
  
  
  inline bool fileExists(const std::string& fileName)
  {
    std::ifstream chunkFile(fileName.c_str());

    return chunkFile.is_open();
  }
}

#endif /* FILESYSTEM_HPP */
  