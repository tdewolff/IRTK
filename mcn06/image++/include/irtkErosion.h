/*=========================================================================

  Library   : Image Registration Toolkit (IRTK)
  Module    : $Id$
  Copyright : Imperial College, Department of Computing
              Visual Information Processing (VIP), 2008 onwards
  Date      : $Date$
  Version   : $Revision$
  Changes   : $Author$

=========================================================================*/

#ifndef _IRTKEROSION_H

#define _IRTKEROSION_H

#include <irtkImageToImage.h>

/**
 * Class for erosion of images
 *
 * This class defines and implements the morphological erosion of images.
 *
 */

template <class VoxelType> class irtkErosion : public irtkImageToImage<VoxelType>
{

protected:

  /// Returns whether the filter requires buffering
  virtual Bool RequiresBuffering();

  /// Returns the name of the class
  virtual const char *NameOfClass();

public:

  /// Constructor
  irtkErosion();

  /// Destructor
  ~irtkErosion();

  /// Run erosion
  virtual void Run();

};

#endif