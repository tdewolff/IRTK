/*=========================================================================

  Library   : Image Registration Toolkit (IRTK)
  Module    : $Id$
  Copyright : Imperial College, Department of Computing
              Visual Information Processing (VIP), 2008 onwards
  Date      : $Date$
  Version   : $Revision$
  Changes   : $Author$

Copyright (c) IXICO LIMITED
All rights reserved.
See COPYRIGHT for details

=========================================================================*/

#include <ctime>
#include <irtkImage.h>

#include <irtkTransformation.h>

// Default filenames
char *image_name = NULL, *input_name = NULL, *output_name = NULL;

void usage()
{
  cerr << "Usage: dof2f [image] [dof] [output] <options>\n" << endl;
  cerr << "where <options> is one or more of the following:\n" << endl;
  cerr << "<-stride value>            Stride value for resampling" << endl;
  cerr << "<-invert>                  Store the inverted displacement field" << endl;
  cerr << "<-Rx1 value>               Region of interest in image" << endl;
  cerr << "<-Ry1 value>               Region of interest in image" << endl;
  cerr << "<-Rz1 value>               Region of interest in image" << endl;
  cerr << "<-Rx2 value>               Region of interest in image" << endl;
  cerr << "<-Ry2 value>               Region of interest in image" << endl;
  cerr << "<-Rz2 value>               Region of interest in image" << endl;
  exit(1);
}

int main(int argc, char **argv)
{
  int x, y, z, t, invert, stride, ok;
  int x1, y1, z1, t1, x2, y2, z2, t2;
  double p0[3];

  if (argc < 4) {
    usage();
  }

  // Parse file names
  image_name  = argv[1];
  argc--;
  argv++;
  input_name  = argv[1];
  argc--;
  argv++;
  output_name = argv[1];
  argc--;
  argv++;

  // Read image
  irtkGreyImage image(image_name);

  // Read transformation
  irtkTransformation *transform = irtkTransformation::New(input_name);

  // Fix ROI
  x1 = 0;
  y1 = 0;
  z1 = 0;
  t1 = 0;
  x2 = image.GetX();
  y2 = image.GetY();
  z2 = image.GetZ();
  t2 = image.GetT();

  // Initialize parameters
  invert = false;
  stride = 1;

  // Parse arguments
  while (argc > 1) {
    ok = false;
    if ((ok == false) && (strcmp(argv[1], "-invert") == 0)) {
      argc--;
      argv++;
      invert = true;
      ok = true;
    }
    if ((ok == false) && (strcmp(argv[1], "-stride") == 0)) {
      argc--;
      argv++;
      stride = atoi(argv[1]);
      argc--;
      argv++;
      ok = true;
    }
    if ((ok == false) && (strcmp(argv[1], "-Rx1") == 0)) {
      argc--;
      argv++;
      x1 = atoi(argv[1]);
      argc--;
      argv++;
      ok = true;
    }
    if ((ok == false) && (strcmp(argv[1], "-Rx2") == 0)) {
      argc--;
      argv++;
      x2 = atoi(argv[1]);
      argc--;
      argv++;
      ok = true;
    }
    if ((ok == false) && (strcmp(argv[1], "-Ry1") == 0)) {
      argc--;
      argv++;
      y1 = atoi(argv[1]);
      argc--;
      argv++;
      ok = true;
    }
    if ((ok == false) && (strcmp(argv[1], "-Ry2") == 0)) {
      argc--;
      argv++;
      y2 = atoi(argv[1]);
      argc--;
      argv++;
      ok = true;
    }
    if ((ok == false) && (strcmp(argv[1], "-Rz1") == 0)) {
      argc--;
      argv++;
      z1 = atoi(argv[1]);
      argc--;
      argv++;
      ok = true;
    }
    if ((ok == false) && (strcmp(argv[1], "-Rz2") == 0)) {
      argc--;
      argv++;
      z2 = atoi(argv[1]);
      argc--;
      argv++;
      ok = true;
    }
    if ((ok == false) && (strcmp(argv[1], "-Rt1") == 0)) {
      argc--;
      argv++;
      t1 = atoi(argv[1]);
      argc--;
      argv++;
      ok = true;
    }
    if ((ok == false) && (strcmp(argv[1], "-Rt2") == 0)) {
      argc--;
      argv++;
      t2 = atoi(argv[1]);
      argc--;
      argv++;
      ok = true;
    }
    if (ok == false) {
      cerr << "Can not parse argument " << argv[1] << endl;
      usage();
    }
  }

  // If there is an region of interest, use it
  if ((x1 != 0) || (x2 != image.GetX()) ||
      (y1 != 0) || (y2 != image.GetY()) ||
      (z1 != 0) || (z2 != image.GetZ()) ||
      (t1 != 0) || (t2 != image.GetZ())) {
    image = image.GetRegion(x1, y1, z1, t1, x2, y2, z2, t2);
  }

  int xMax = image.GetX() / stride;
  int yMax = image.GetY() / stride;
  int zMax = image.GetZ() / stride;

  double psx, psy, psz;
  image.GetPixelSize(&psx, &psy, &psz);
  psx *= stride;
  psy *= stride;
  psz *= stride;

  irtkRealImage f11(xMax, yMax, zMax);
  irtkRealImage f12(xMax, yMax, zMax);
  irtkRealImage f13(xMax, yMax, zMax);
  irtkRealImage f21(xMax, yMax, zMax);
  irtkRealImage f22(xMax, yMax, zMax);
  irtkRealImage f23(xMax, yMax, zMax);
  irtkRealImage f31(xMax, yMax, zMax);
  irtkRealImage f32(xMax, yMax, zMax);
  irtkRealImage f33(xMax, yMax, zMax);
  f11.PutPixelSize(psx, psy, psz);
  f12.PutPixelSize(psx, psy, psz);
  f13.PutPixelSize(psx, psy, psz);
  f21.PutPixelSize(psx, psy, psz);
  f22.PutPixelSize(psx, psy, psz);
  f23.PutPixelSize(psx, psy, psz);
  f31.PutPixelSize(psx, psy, psz);
  f32.PutPixelSize(psx, psy, psz);
  f33.PutPixelSize(psx, psy, psz);

  std::cout << "Use stride = " << stride << std::endl;

  // Initialize point structure with transformed point positions
  int i, j, k;
  std::clock_t begin = std::clock();
  std::cout.precision(3);
  for (t = 0; t < image.GetT(); t++) {
    for (z = 0; z < zMax; z++) {
      if (z == 0) {
        std::cout << "Calculating 1/" << zMax << std::endl;
      } else {
        double mins = double(std::clock() - begin) / CLOCKS_PER_SEC / 60.0;
        mins *= double(zMax-z)/z;
        std::cout << "Calculating " << (z+1) << "/" << zMax << " -- " << mins << " min remaining" << std::endl;
      }
      for (y = 0; y < yMax; y++) {
        for (x = 0; x < xMax; x++) {
          double x1, y1, z1, x2, y2, z2, d;
          irtkMatrix F(3,3), FLocal(3,3);

          for (k = 0; k < stride; k++) {
            for (j = 0; j < stride; j++) {
              for (i = 0; i < stride; i++) {
                p0[0] = x*stride+i;
                p0[1] = y*stride+j;
                p0[2] = z*stride+k;

                // Take derivates wrt x, y, and z, and construct the deformation tensor.
                x1 = p0[0]+0.5;
                y1 = p0[1];
                z1 = p0[2];
                x2 = p0[0]-0.5;
                y2 = p0[1];
                z2 = p0[2];
                image.ImageToWorld(x1, y1, z1);
                image.ImageToWorld(x2, y2, z2);
                d = x1-x2;
                transform->Transform(x1, y1, z1);
                transform->Transform(x2, y2, z2);
                FLocal(0,0) = (x1-x2)/d;
                FLocal(1,0) = (y1-y2)/d;
                FLocal(2,0) = (z1-z2)/d;

                x1 = p0[0];
                y1 = p0[1]+0.5;
                z1 = p0[2];
                x2 = p0[0];
                y2 = p0[1]-0.5;
                z2 = p0[2];
                image.ImageToWorld(x1, y1, z1);
                image.ImageToWorld(x2, y2, z2);
                d = y1-y2;
                transform->Transform(x1, y1, z1);
                transform->Transform(x2, y2, z2);
                FLocal(0,1) = (x1-x2)/d;
                FLocal(1,1) = (y1-y2)/d;
                FLocal(2,1) = (z1-z2)/d;

                x1 = p0[0];
                y1 = p0[1];
                z1 = p0[2]+0.5;
                x2 = p0[0];
                y2 = p0[1];
                z2 = p0[2]-0.5;
                image.ImageToWorld(x1, y1, z1);
                image.ImageToWorld(x2, y2, z2);
                d = z1-z2;
                transform->Transform(x1, y1, z1);
                transform->Transform(x2, y2, z2);
                FLocal(0,2) = (x1-x2)/d;
                FLocal(1,2) = (y1-y2)/d;
                FLocal(2,2) = (z1-z2)/d;

                if (x == floor(xMax/2) && y == floor(yMax/2) && z == floor(zMax/2)) {
                    std::cout << "\nF at image centre (" << x << "," << y << "," << z << "):" << std::endl;
                    std::cout << "  from dx/dX" << std::endl;
                    FLocal.Print();

                    std::cout << "  from IRTK Jacobian method" << std::endl;
                    irtkMatrix jacLocal(3,3);
                    image.ImageToWorld(p0[0], p0[1], p0[2]);
                    transform->Jacobian(jacLocal, p0[0], p0[1], p0[2]);
                    jacLocal.Print();
                    std::cout << std::endl;
                }

                if (invert == true) {
                    FLocal.Invert();
                }
                F += FLocal;
              }
            }
          }
          F /= stride * stride * stride;

          f11(x, y, z, t) = F(0,0);
          f12(x, y, z, t) = F(1,0);
          f13(x, y, z, t) = F(2,0);
          f21(x, y, z, t) = F(0,1);
          f22(x, y, z, t) = F(1,1);
          f23(x, y, z, t) = F(2,1);
          f31(x, y, z, t) = F(0,2);
          f32(x, y, z, t) = F(1,2);
          f33(x, y, z, t) = F(2,2);
        }
      }
    }
  }

  char *output_name_f = (char *)malloc(strlen(output_name)+8);
  strcpy(output_name_f, output_name);
  strcpy(output_name_f + strlen(output_name), "_f00.nii\0");

  output_name_f[strlen(output_name_f)-6] = '1';
  output_name_f[strlen(output_name_f)-5] = '1';
  std::cout << "Writing f11" << std::endl;
  f11.Write(output_name_f);
  output_name_f[strlen(output_name_f)-6] = '1';
  output_name_f[strlen(output_name_f)-5] = '2';
  std::cout << "Writing f12" << std::endl;
  f12.Write(output_name_f);
  output_name_f[strlen(output_name_f)-6] = '1';
  output_name_f[strlen(output_name_f)-5] = '3';
  std::cout << "Writing f13" << std::endl;
  f13.Write(output_name_f);
  output_name_f[strlen(output_name_f)-6] = '2';
  output_name_f[strlen(output_name_f)-5] = '1';
  std::cout << "Writing f21" << std::endl;
  f21.Write(output_name_f);
  output_name_f[strlen(output_name_f)-6] = '2';
  output_name_f[strlen(output_name_f)-5] = '2';
  std::cout << "Writing f22" << std::endl;
  f22.Write(output_name_f);
  output_name_f[strlen(output_name_f)-6] = '2';
  output_name_f[strlen(output_name_f)-5] = '3';
  std::cout << "Writing f23" << std::endl;
  f23.Write(output_name_f);
  output_name_f[strlen(output_name_f)-6] = '3';
  output_name_f[strlen(output_name_f)-5] = '1';
  std::cout << "Writing f31" << std::endl;
  f31.Write(output_name_f);
  output_name_f[strlen(output_name_f)-6] = '3';
  output_name_f[strlen(output_name_f)-5] = '2';
  std::cout << "Writing f32" << std::endl;
  f32.Write(output_name_f);
  output_name_f[strlen(output_name_f)-6] = '3';
  output_name_f[strlen(output_name_f)-5] = '3';
  std::cout << "Writing f33" << std::endl;
  f33.Write(output_name_f);
}
