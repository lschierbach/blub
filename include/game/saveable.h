#ifndef SAVEABLE_H
#define SAVEABLE_H

#include <fstream>
#include <ostream>

struct Saveable
{
  virtual void write(std::ofstream& out) = 0;
  virtual void read(std::ifstream& in) = 0;
  
  friend std::ofstream& operator<< (std::ofstream& out, Saveable& saveable) 
  { 
    saveable.write(out); 
    return out; 
  };
  
  friend std::ifstream& operator>> (std::ifstream& in, Saveable& saveable)
  { 
    saveable.read(in);
    return in;
  };
};

#endif /* SAVEABLE_H */