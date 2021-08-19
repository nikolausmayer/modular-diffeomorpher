
/**
 * Nikolaus Mayer, 2017 (mayern@cs.uni-freiburg.de)
 */

/// System/STL
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
/// Local files
#include "CImg.h"

using namespace cimg_library;


int main(int argc, char** argv)
{
  if (argc < 3) {
    std::cout << "Usage: " << argv[0]
                           << " <image> <flow.pfm> [inverse] [output]"
                           << std::endl;
    return EXIT_FAILURE;
  }

  std::string infile, flowfile, outfile;
  bool inverse = false, output = false;
  {
    uint8_t state{0};
    for (unsigned int i = 1; i < static_cast<unsigned int>(argc); ++i) {
      if (std::strcmp(argv[i], "inverse")==0)
        inverse = true;
      else switch (state) {
        case 0: { infile   = argv[i]; ++state; break; }
        case 1: { flowfile = argv[i]; ++state; break; }
        case 2: { outfile  = argv[i]; output = true; ++state; break; }
        default: { std::cout << "Unused parameter: " << argv[i] << std::endl; }
      }
    }
  }


  CImg<unsigned char> image;
  image.load(infile.c_str());

  CImg<float> flow;
  flow.load_pfm(flowfile.c_str());

  CImg<unsigned char> result(image);

  if (inverse) {
    /// Backward warping (no gaps)
    result.fill(static_cast<unsigned char>(0));

    const unsigned char zero{0};
    cimg_forXY(result, x, y){
      const float xf = x+flow(x,y,0);
      const float yf = y+flow(x,y,1);
      result(x,y,0) = image.linear_atXY(xf,yf,0,0,zero);
      result(x,y,1) = image.linear_atXY(xf,yf,0,1,zero);
      result(x,y,2) = image.linear_atXY(xf,yf,0,2,zero);
    }
  } else {
    /// Forward warping
    CImg<float> intermediate(image.width(), image.height(), 1, 4);
    intermediate.fill(0.f);

    cimg_forXY(flow, x, y){
      const float xflow = flow(x,y,0);
      const float yflow = flow(x,y,1);
      const float target_x = x+xflow;
      const float target_y = y+yflow;
      const int xi = std::floor(target_x);
      const int yi = std::floor(target_y);
      const float xf = target_x-xi;
      const float yf = target_y-yi;

      if (xi < 0 or xi >= image.width() or
          yi < 0 or yi >= image.height())
        continue;

      float weight = (1.f-xf)*(1.f-yf);
      for (int c = 0; c < 3; ++c)
        intermediate(xi,yi,c) += weight*image(x,y,c);
      intermediate(xi,yi,3) += weight;

      if (xi < image.width()-1) {
        weight = (    xf)*(1.f-yf);
        for (int c = 0; c < 3; ++c)
          intermediate(xi+1,yi,c) += weight*image(x,y,c);
        intermediate(xi+1,yi,3) += weight;
      }
     
      if (yi < image.height()-1) {
        weight = (1.f-xf)*(    yf);
        for (int c = 0; c < 3; ++c)
          intermediate(xi,yi+1,c) += weight*image(x,y,c);
        intermediate(xi,yi+1,3) += weight;
      }

      if (xi < image.width()-1 and yi < image.height()-1) {
        weight = (    xf)*(    yf);
        for (int c = 0; c < 3; ++c)
          intermediate(xi+1,yi+1,c) += weight*image(x,y,c);
        intermediate(xi+1,yi+1,3) += weight;
      }
    }


    result.fill(static_cast<unsigned char>(0));
    cimg_forXY(result, x, y){
      const float weight = intermediate(x,y,3);
      result(x,y,0) = static_cast<unsigned char>(intermediate(x,y,0)/weight);
      result(x,y,1) = static_cast<unsigned char>(intermediate(x,y,1)/weight);
      result(x,y,2) = static_cast<unsigned char>(intermediate(x,y,2)/weight);
    }
  }

  if (output) {
    result.save(outfile.c_str());
  } else {
    CImgDisplay display;
    display.display(result);
    while (not display.is_closed()) {
      if (display.is_keyESC() or 
          display.is_keyX() or
          display.is_keyQ())
        break;
      display.wait_all();
    }
  }

  /// Bye!
  return EXIT_SUCCESS;
}

