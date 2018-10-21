/*
 *  FILENAME:      main.cpp
 *
 *  DESCRIPTION:
 *      Main file of project, initialise graphics and run input/render loop
 *
 *  NOTES:
 *    
 *
 *  AUTHOR:        Tobias Fey     DATE: 01.10.2018
 *
 *  CHANGES:
 *
 *  TODO:
 *    -Change debug printf into log-system
 */

#include <iostream>
#include <getopt.h>

#include "SDL_gpu.h"

#include "controller.h"

int main(int argc, char** argv)
{
  std::cout << "Program starting up" << std::endl;
  
  Controller ctrl;
 
  ctrl.init();
    
  do{ }while(ctrl.tick());

  GPU_Quit();

  std::cout << "done" << std::endl;

  return EXIT_SUCCESS;
}
