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
 *      void        template  writeStruct(std::ofstream&, Type) -> isSaveable
 *      void        template  writeStruct(std::ofstream&, Type) -> !isSaveable
 *      void        template  writeStruct(const std::string&, Type)
 *
 *      void        template  writeContainer(std::ofstream&, Type) -> isrange
 *      void        template  writeContainer(std::ofstream&, Type) -> !isrange
 *      void        template  writeContainer(const std::string&, Type)
 *
 *      void        template  readStruct(std::ifstream&, Type) -> isSaveable
 *      void        template  readStruct(std::ifstream&, Type) -> !isSaveable
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
 *      
 */
#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <vector>
#include <fstream>
#include <ostream>

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
struct has_write : public std::false_type {};

template <typename T>
struct has_write<T, void_t<decltype(std::declval<T>().write(std::declval<std::ofstream&>()))>> : public std::true_type {};

template <typename T, typename = void>
struct has_read : public std::false_type {};

template <typename T>
struct has_read<T, void_t<decltype(std::declval<T>().read(std::declval<std::ifstream&>()))>> : public std::true_type {};



template <typename Type>
struct is_range : public std::integral_constant<bool, (has_size<Type>::value && is_iterable<Type>::value)> {};

template <typename Type>
struct is_saveable : public std::integral_constant<bool, (has_write<Type>::value && has_read<Type>::value)> {};


template< bool B, class T = void >
using enable_if_t = typename std::enable_if<B,T>::type;

namespace filesystem
{
  
  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   *                                           WRITE_STRUCT                                            *
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  ** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
  
  
  /**
    Writes a saveable struct. A struct is saveable if it contains the methods "write" and "read". It will then manage the writeprocess itself.
    
    @param out              ofstream which should be written in
    @param saveableStruct   the struct which should be saved
  */
  template<typename Struct>
  auto writeStruct(std::ofstream& out, Struct& saveableStruct) -> enable_if_t<is_saveable<Struct>::value>
  {
    saveableStruct.write(out);
  }
  
  /**
    Writes a nonsaveable struct. A struct is non-saveable if it doesn't contain the methods "write" and "read". its data will be written plain
    
    Note: If the struct to be written contains non-pod (e.g. strings, arrays..) it will lose its data.
    
    @param out              ofstream which should be written in
    @param podStruct        the struct which should be saved
  */
  template<typename Struct>
  auto writeStruct(std::ofstream& out, Struct& podStruct) -> enable_if_t<!is_saveable<Struct>::value>
  {
    //printf("Wrirw Struct, size %u\n", sizeof(Struct));
    out.write(reinterpret_cast<const char*>(&podStruct), sizeof(Struct));
  }
  
  /**
    Helper method for function write-calls without a given ofstream.
    
    @param filePath         filePath
    @param strct            a struct
  */
  template<typename Struct>
  void writeStruct(const std::string& filePath, Struct& strct) 
  {
    std::ofstream fstream(filePath, std::ios::out | std::ios::binary);
    writeStruct(fstream, strct);
  }

  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   *                                           WRITE_RANGE                                             *
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  ** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
  
  /**
    Is called if the given struct isn't a range.
    
    @param out              stream for writing
    @param strct            a struct
  */
  template <typename Struct>
  auto writeRange(std::ofstream& out, Struct& strct, int i = 0) -> enable_if_t<!is_range<Struct>::value>
  {
    writeStruct(out, strct);
  }

  /**
    Is called if the given struct is a range. Calls "writeRange" for each element of the range.
    
    @param out              stream for writing
    @param currentRange     range
  */
  template<typename Range>
  auto writeRange(std::ofstream& out, Range& currentRange, int i = 0) -> enable_if_t<is_range<Range>::value>
  {
    // first save size
    size_t size = currentRange.size();
    writeStruct<size_t>(out, size);
    
    //printf("Write Long (Size: %u, Content: %ul, dimension %i)\n", sizeof(size), size, i);
    
    // then content (recursive)
    for(auto& elem : currentRange)
    {
      writeRange(out, elem, i + 1);
    }
  }
  
  /**
    Helper method for function write-calls without a given ofstream.
    
    @param filePath         filePath
    @param range            range
  */
  template <typename Range>
  void writeRange(const std::string& filePath, Range& range, int i = 0)
  {
    std::ofstream fstream(filePath, std::ios::out | std::ios::binary);
    writeRange(fstream, range, i);
  }
  
  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   *                                           READ_STRUCT                                             *
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  ** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
  
  
  /**
    Reads a saveable struct. A struct is saveable if it contains the methods "write" and "read". It will then manage the writeprocess itself.
    
    @param in               ifstream which should be read in
    @param saveableStruct   the struct which should be read
  */
  template<typename Struct>
  auto readStruct(std::ifstream& in, Struct& saveableStruct) -> enable_if_t<is_saveable<Struct>::value>
  {
    saveableStruct.read(in);
  }
  
  /**
    Reads a nonsaveable struct. A struct is non-saveable if it doesn't contain the methods "write" and "read". its data will be read plain
    
    Note: If the struct to be read contains non-pod (e.g. strings, arrays..) it will give unpredictable results.
    
    @param out              ifstream which should be read in
    @param podStruct        the struct which should be read
  */
  template<typename Struct>
  auto readStruct(std::ifstream& in, Struct& podStruct) -> enable_if_t<!is_saveable<Struct>::value>
  {
    //printf("Read Struct, size %u\n", sizeof(Struct));
    in.read(reinterpret_cast<char*>(&podStruct), sizeof(Struct));
  }
  
  /**
    Helper method for function read-calls without a given ifstream.
    
    @param filePath         filePath
    @param strct            a struct
  */
  template<typename Struct>
  void readStruct(const std::string& filePath, Struct& strct) 
  {
    std::ifstream fstream(filePath, std::ios::in | std::ios::binary);
    readStruct(fstream, strct);
  }

  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   *                                           READ_RANGE                                              *
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  ** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
  
  
  /**
    Is called if the given struct isn't a range.
    
    @param in               stream for readin
    @param strct            a struct
  */
  template<typename Struct>
  auto readRange(std::ifstream& in, Struct& strct, int i = 0) -> enable_if_t<!is_range<Struct>::value>
  {
    readStruct(in, strct);
  }
  
  /**
    Is called if the given struct isn't a range. Calls "readRange" for each element of the range.
    
    @param out              stream for writing
    @param currentRange     range
  */
  template<typename Range>
  auto readRange(std::ifstream& in, Range& range, int d = 0) -> enable_if_t<is_range<Range>::value>
  {
    // first get size
    size_t size;
    readStruct<size_t>(in, size);
    
    //printf("Read Long (Size: %u, Content: %ul, Dimension %i)\n", sizeof(size), size, d);
    
    // then content
    for (auto i = 0; i < size; i++)
    {
      typename Range::value_type elem;
      readRange(in, elem, d+1);
      range.push_back(elem);
    }
  }
  
  /**
    Helper method for function read-calls without a given ifstream.
    
    @param filePath         filePath
    @param range            range
  */
  template<typename Range>
  void readRange(const std::string& filePath, Range& range, int i = 0)
  {
    std::ifstream filestream(filePath, std::ios::in | std::ios::binary);
    readRange(filestream, range, i);
  }
  
  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
   *                                           NON_TEMPLATE                                            *
   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  ** * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
  
  
  inline bool fileExists(const std::string& fileName)
  {
    std::ifstream chunkFile(fileName.c_str());

    return chunkFile.is_open();
  }
}

#endif /* FILESYSTEM_HPP */
  