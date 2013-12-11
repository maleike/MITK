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

#include "mitkDICOMGDCMImageFrameInfo.h"

mitk::DICOMGDCMImageFrameInfo
::DICOMGDCMImageFrameInfo(const std::string& filename, unsigned int frameNo)
:itk::LightObject()
,m_FrameInfo( DICOMImageFrameInfo::New(filename, frameNo) )
,m_TagForValue(gdcm::Scanner::TagToValue()) // empty
{
}

mitk::DICOMGDCMImageFrameInfo
::DICOMGDCMImageFrameInfo(DICOMImageFrameInfo::Pointer frameinfo)
:itk::LightObject()
,m_FrameInfo(frameinfo)
,m_TagForValue(gdcm::Scanner::TagToValue()) // empty
{
}

mitk::DICOMGDCMImageFrameInfo
::DICOMGDCMImageFrameInfo(DICOMImageFrameInfo::Pointer frameinfo, gdcm::Scanner::TagToValue const& tagToValueMapping)
:itk::LightObject()
,m_FrameInfo(frameinfo)
,m_TagForValue(tagToValueMapping)
{
}


std::string
mitk::DICOMGDCMImageFrameInfo
::GetTagValueAsString(const DICOMTag& tag) const
{
  gdcm::Scanner::TagToValue::const_iterator mappedValue = m_TagForValue.find( gdcm::Tag(tag.GetGroup(), tag.GetElement()) );

  if (mappedValue != m_TagForValue.end())
  {
    return mappedValue->second;
  }
  else
  {
    return std::string("");
  }
}

std::string
mitk::DICOMGDCMImageFrameInfo
::GetFilenameIfAvailable() const
{
  if (m_FrameInfo.IsNotNull())
  {
    return m_FrameInfo->Filename;
  }
  else
  {
    return "";
  }
}

mitk::DICOMImageFrameInfo::Pointer
mitk::DICOMGDCMImageFrameInfo
::GetFrameInfo() const
{
  return m_FrameInfo;
}

void
  mitk::DICOMGDCMImageFrameInfo
::SetFrameInfo(DICOMImageFrameInfo::Pointer frameinfo)
{
  m_FrameInfo = frameinfo;
}
