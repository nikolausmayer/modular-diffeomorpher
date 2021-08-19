/**
 * Nikolaus Mayer, 2017 (mayern@cs.uni-freiburg.de)
 */

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <string>
#include <tuple>

#include "CImg.h"
#include "WarpFields.h"

using namespace cimg_library;




int main(int argc, char** argv)
{
  (void)argc; (void)argv;

  WarpFields::DisplacementComposer dc(512, 384);

  // dc.add_displacer(new WarpFields::Displacers::Rotation(256, 192, 0.0000001))
  //   .with_support(new WarpFields::Supports::Constant());
  
  dc.add_displacer(new WarpFields::Displacers::Translation(0.0005, 0.00025))
     .with_support(new WarpFields::Supports::Gaussian1D(255.5, 191.5, 50))
    .add_displacer(new WarpFields::Displacers::Translation(-0.0003,-0.00015))
     .with_support(new WarpFields::Supports::Gaussian1D(300, 200, 30))
    .add_displacer(new WarpFields::Displacers::Rotation(100, 50, 0.00001))
     .with_support(new WarpFields::Supports::Gaussian1D(100, 50, 75))
    .add_displacer(new WarpFields::Displacers::Rotation(100, 300, 0.00002))
     .with_support(new WarpFields::Supports::Gaussian1D(100, 300, 50))
    .add_displacer(new WarpFields::Displacers::Zoom(450, 130, 1.f+2e-6))
     .with_support(new WarpFields::Supports::Gaussian1D(450, 130, 25))
    .add_displacer(new WarpFields::Displacers::Zoom(500, 380, 1.f-2e-6))
     .with_support(new WarpFields::Supports::Gaussian1D(500, 380, 50)) 
    .add_displacer(new WarpFields::Displacers::Rotation(255.5, 191.5, 0.00002))
     .with_support(new WarpFields::Supports::Gaussian2D(255.5, 191.5, 75, 25, M_PI/4.));

  WarpFields::FlowField ff;
  ff.init_from_DisplacementComposer(dc).clamp_near_zeros();
  ff.get_flow().save("flow.pfm");
  ff.get_iflow().save("iflow.pfm");

  /**
   * The "WarpFields" class has an asynchronous "CropGenerator" function where
   * larger fields are generated, and smaller crops can be requested. This can
   * be much faster if absolute uniqueness of the flow fields is not needed.
   * However, currently the way in which the large flow field is generated is
   * hardcoded and not parameterized, so it lacks user-friendly flexibility.
   * The "big" flow field is 3*max(crop width, crop height) in each dimension.
   *
   * CropGenerator usage:
   *
   * /// crop width, crop height, how often the same crop may be reused
   * WarpFields::CropGenerator m_crop_generator(512, 384, 2);
   * m_crop_generator.Start();
   * CImg<float> forward, backward;
   * std::tie(forward, backward) = m_crop_generator.get_crop();
   * ...
   * m_crop_generator.Stop();
   */

  return EXIT_SUCCESS;
}

