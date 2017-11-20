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

#include "mitkNavigationTool.h"
#include "mitkIGTException.h"
#include "mitkNavigationData.h"
#include "Poco/File.h"
#include "mitkUnspecifiedTrackingTypeInformation.h"
#include "mitkInternalTrackingTool.h"

#include "vtkSphereSource.h"

mitk::NavigationTool::NavigationTool() : m_Identifier("None"),
m_Type(mitk::NavigationTool::Unknown),
m_CalibrationFile("none"),
m_SerialNumber(""),
m_TrackingDeviceType(mitk::UnspecifiedTrackingTypeInformation::GetTrackingDeviceName()),
m_ToolRegistrationLandmarks(mitk::PointSet::New()),
m_ToolCalibrationLandmarks(mitk::PointSet::New()),
m_ToolTipOrientation(mitk::Quaternion(0, 0, 0, 1))
{
  m_ToolTipPosition[0] = 0;
  m_ToolTipPosition[1] = 0;
  m_ToolTipPosition[2] = 0;

  m_ToolAxis[0] = 1;
  m_ToolAxis[1] = 0;
  m_ToolAxis[2] = 0;

  SetDefaultSurface();
}

itk::LightObject::Pointer mitk::NavigationTool::InternalClone() const
{
  Self::Pointer tool = new Self(*this);
  tool->UnRegister();
  return tool.GetPointer();
}

mitk::NavigationTool::NavigationTool(const NavigationTool &other)
  : Superclass()
{
  this->m_Identifier = other.m_Identifier;
  this->m_Type = other.m_Type;
  if (other.m_DataNode.IsNotNull())
  {
    this->m_DataNode = other.m_DataNode->Clone();
    this->m_DataNode->SetName(other.m_DataNode->GetName());
    if (other.m_DataNode->GetData())
    {
      this->m_DataNode->SetData(dynamic_cast<mitk::BaseData*>(other.m_DataNode->GetData()->Clone().GetPointer()));
    }
  }

  if (other.m_SpatialObject.IsNotNull())
    this->m_SpatialObject = other.m_SpatialObject->Clone();
  this->m_CalibrationFile = other.m_CalibrationFile;
  this->m_SerialNumber = other.m_SerialNumber;
  this->m_TrackingDeviceType = other.m_TrackingDeviceType;
  if (other.m_ToolRegistrationLandmarks.IsNotNull())
    this->m_ToolRegistrationLandmarks = other.m_ToolRegistrationLandmarks->Clone();
  if (other.m_ToolCalibrationLandmarks.IsNotNull())
    this->m_ToolCalibrationLandmarks = other.m_ToolCalibrationLandmarks->Clone();
  this->m_ToolTipPosition = other.m_ToolTipPosition;
  this->m_ToolTipOrientation = other.m_ToolTipOrientation;
  this->m_ToolAxis = other.m_ToolAxis;
}

mitk::NavigationTool::~NavigationTool()
{
}

mitk::AffineTransform3D::Pointer mitk::NavigationTool::GetToolTipTransform()
{
  mitk::NavigationData::Pointer returnValue = mitk::NavigationData::New();
  returnValue->SetPosition(this->m_ToolTipPosition);
  returnValue->SetOrientation(this->m_ToolTipOrientation);
  return returnValue->GetAffineTransform3D();
}

void mitk::NavigationTool::Graft(const DataObject *data)
{
  // Attempt to cast data to an NavigationData
  const Self* nd;
  try
  {
    nd = dynamic_cast<const Self *>(data);
  }
  catch (...)
  {
    mitkThrowException(mitk::IGTException) << "mitk::NavigationData::Graft cannot cast "
      << typeid(data).name() << " to "
      << typeid(const Self *).name();
  }
  if (!nd)
  {
    // pointer could not be cast back down
    mitkThrowException(mitk::IGTException) << "mitk::NavigationData::Graft cannot cast "
      << typeid(data).name() << " to "
      << typeid(const Self *).name();
  }
  // Now copy anything that is needed
  m_Identifier = nd->GetIdentifier();
  m_Type = nd->GetType();
  m_DataNode->SetName(nd->GetDataNode()->GetName());
  m_DataNode->SetData(nd->GetDataNode()->GetData());
  m_SpatialObject = nd->GetSpatialObject();
  m_CalibrationFile = nd->GetCalibrationFile();
  m_SerialNumber = nd->GetSerialNumber();
  m_TrackingDeviceType = nd->GetTrackingDeviceType();
  m_ToolRegistrationLandmarks = nd->GetToolRegistrationLandmarks();
  m_ToolCalibrationLandmarks = nd->GetToolCalibrationLandmarks();
  m_ToolTipPosition = nd->GetToolTipPosition();
  m_ToolTipOrientation = nd->GetToolTipOrientation();
  m_ToolAxis = nd->GetToolAxis();
}

bool mitk::NavigationTool::IsToolTipSet()
{
  if ((m_ToolTipPosition[0] == 0) &&
    (m_ToolTipPosition[1] == 0) &&
    (m_ToolTipPosition[2] == 0) &&
    (m_ToolTipOrientation.x() == 0) &&
    (m_ToolTipOrientation.y() == 0) &&
    (m_ToolTipOrientation.z() == 0) &&
    (m_ToolTipOrientation.r() == 1))
    return false;
  else return true;
}

void mitk::NavigationTool::SetCalibrationFile(const std::string filename)
{
  //check if file does exist:
  if (filename == "")
  {
    m_CalibrationFile = "none";
  }
  else
  {
    Poco::File myFile(filename);
    if (myFile.exists())
      m_CalibrationFile = filename;
    else
      m_CalibrationFile = "none";
  }
}

std::string mitk::NavigationTool::GetToolName()
{
  if (this->m_DataNode.IsNull()) { return ""; }
  else { return m_DataNode->GetName(); }
}

mitk::Surface::Pointer mitk::NavigationTool::GetToolSurface()
{
  if (this->m_DataNode.IsNull()) { return nullptr; }
  else if (this->m_DataNode->GetData() == nullptr) { return nullptr; }
  else { return dynamic_cast<mitk::Surface*>(m_DataNode->GetData()); }
}

void mitk::NavigationTool::SetDefaultSurface()
{
  if (m_DataNode.IsNull())
    m_DataNode = mitk::DataNode::New();

  //create small cone and use it as surface
  mitk::Surface::Pointer mySphere = mitk::Surface::New();
  vtkSphereSource *vtkData = vtkSphereSource::New();
  vtkData->SetRadius(2.0f);
  vtkData->SetCenter(0.0, 0.0, 0.0);
  vtkData->Update();
  mySphere->SetVtkPolyData(vtkData->GetOutput());
  vtkData->Delete();
  this->GetDataNode()->SetData(mySphere);
}

std::string mitk::NavigationTool::GetStringWithAllToolInformation() const
{
  std::stringstream _info;
  _info << "  Identifier: " << this->m_Identifier << "\n"
    << "  NavigationToolType: " << m_Type << "\n"
    << "  Calibration file: " << m_CalibrationFile << "\n"
    << "  Serial number: " << m_SerialNumber << "\n"
    << "  TrackingDeviceType: " << m_TrackingDeviceType << "\n"
    << "  ToolTip Position: " << m_ToolTipPosition << "\n"
    << "  ToolTip Orientation: " << m_ToolTipOrientation << "\n"
    << "  ToolTip Axis: " << m_ToolAxis;

  return _info.str();
}