/*=========================================================================

Program:   Medical Imaging & Interaction Toolkit
Language:  C++
Date:      $Date$
Version:   $Revision$

Copyright (c) German Cancer Research Center, Division of Medical and
Biological Informatics. All rights reserved.
See MITKCopyright.txt or http://www.mitk.org/copyright.html for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#ifndef MITKFOOSERVICE_H
#define MITKFOOSERVICE_H

#include <mitkServiceInterface.h>

namespace mitk {

struct FooService
{
  virtual ~FooService() {}
  virtual void foo() = 0;
};

}

MITK_DECLARE_SERVICE_INTERFACE(mitk::FooService, "org.mitk.testing.FooService")

#endif // MITKFOOSERVICE_H
