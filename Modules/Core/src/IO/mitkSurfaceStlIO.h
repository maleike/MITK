/*============================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center (DKFZ)
All rights reserved.

Use of this source code is governed by a 3-clause BSD license that can be
found in the LICENSE file.

============================================================================*/

#ifndef _MITK_SURFACE_STL_IO_H_
#define _MITK_SURFACE_STL_IO_H_

#include "mitkSurfaceVtkIO.h"

namespace mitk
{
  class SurfaceStlIO : public mitk::SurfaceVtkIO
  {
  public:
    SurfaceStlIO();

    // -------------- AbstractFileReader -------------

    using AbstractFileReader::Read;

    // -------------- AbstractFileWriter -------------

    void Write() override;

  protected:
    std::vector<BaseData::Pointer> DoRead() override;

  private:
    SurfaceStlIO *IOClone() const override;

    static std::string OPTION_MERGE_POINTS();
    static std::string OPTION_TAG_SOLIDS();
    static std::string OPTION_CLEAN();
  };
}

#endif //_MITK_SURFACE_STL_IO_H_
