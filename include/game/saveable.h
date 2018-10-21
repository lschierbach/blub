#ifndef SAVEABLE_H
#define SAVEABLE_H

struct Saveable
{
  virtual void write(std::ofstream& out) = 0;
  virtual void read(std::ifstream& in) = 0;
};

#endif /* SAVEABLE_H */