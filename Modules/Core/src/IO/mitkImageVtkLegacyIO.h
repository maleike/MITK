/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#ifndef MITKIMAGEVTKLEGACYIO_H
#define MITKIMAGEVTKLEGACYIO_H

#include "mitkAbstractFileIO.h"

namespace mitk
{
  class ImageVtkLegacyIO : public mitk::AbstractFileIO
  {
  public:
    ImageVtkLegacyIO();

    // -------------- AbstractFileReader -------------

    using AbstractFileReader::Read;
    std::vector<BaseData::Pointer> Read() override;

    ConfidenceLevel GetReaderConfidenceLevel() const override;

    // -------------- AbstractFileWriter -------------

    void Write() override;

    ConfidenceLevel GetWriterConfidenceLevel() const override;

  private:
    ImageVtkLegacyIO *IOClone() const override;
  };
}
#endif // MITKIMAGEVTKLEGACYIO_H
