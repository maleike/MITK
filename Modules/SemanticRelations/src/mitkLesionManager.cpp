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

// semantic relations module
#include "mitkLesionManager.h"
#include "mitkRelationStorage.h"
#include "mitkSemanticRelationException.h"
#include "mitkSemanticRelationsInference.h"
#include "mitkUIDGeneratorBoost.h"

mitk::SemanticTypes::Lesion mitk::GenerateNewLesion(const std::string& lesionClassType/* = ""*/)
{
  SemanticTypes::Lesion lesion;
  lesion.UID = mitk::UIDGeneratorBoost::GenerateUID();
  lesion.name = "New lesion";
  lesion.lesionClass = GenerateNewLesionClass(lesionClassType);

  return lesion;
}

mitk::SemanticTypes::LesionClass mitk::GenerateNewLesionClass(const std::string& lesionClassType/* = ""*/)
{
  SemanticTypes::LesionClass lesionClass;
  lesionClass.UID = mitk::UIDGeneratorBoost::GenerateUID();
  lesionClass.classType = lesionClassType;

  return lesionClass;
}

mitk::SemanticTypes::Lesion mitk::GetLesionByUID(const SemanticTypes::ID& lesionUID, const std::vector<SemanticTypes::Lesion>& allLesions)
{
  auto lambda = [&lesionUID](const SemanticTypes::Lesion& currentLesion)
  {
    return currentLesion.UID == lesionUID;
  };

  const auto existingLesion = std::find_if(allLesions.begin(), allLesions.end(), lambda);

  SemanticTypes::Lesion lesion;
  if (existingLesion != allLesions.end())
  {
    lesion = *existingLesion;
  }

  return lesion;
}

mitk::SemanticTypes::LesionClass mitk::FindExistingLesionClass(const std::string& lesionClassType, const std::vector<SemanticTypes::LesionClass>& allLesionClasses)
{
  auto lambda = [&lesionClassType](const SemanticTypes::LesionClass& currentLesionClass)
  {
    return currentLesionClass.classType == lesionClassType;
  };

  const auto existingLesionClass = std::find_if(allLesionClasses.begin(), allLesionClasses.end(), lambda);

  SemanticTypes::LesionClass lesionClass;
  if (existingLesionClass != allLesionClasses.end())
  {
    lesionClass = *existingLesionClass;
  }

  return lesionClass;
}

void mitk::ComputeLesionPresence(LesionData& lesionData, const SemanticTypes::CaseID& caseID)
{
  std::vector<bool> lesionPresence;
  SemanticTypes::Lesion lesion = lesionData.GetLesion();
  bool presence = false;
  SemanticTypes::ControlPointVector controlPoints = RelationStorage::GetAllControlPointsOfCase(caseID);
  // sort the vector of control points for the timeline
  std::sort(controlPoints.begin(), controlPoints.end());
  SemanticTypes::InformationTypeVector informationTypes = mitk::RelationStorage::GetAllInformationTypesOfCase(caseID);
  for (const auto& informationType : informationTypes)
  {
    for (const auto& controlPoint : controlPoints)
    {
      try
      {
        presence = SemanticRelationsInference::IsLesionPresentAtControlPoint(caseID, lesion, controlPoint);
      }
      catch (SemanticRelationException&)
      {
        presence = false;
      }

      lesionPresence.push_back(presence);
    }
  }

  lesionData.SetLesionPresence(lesionPresence);
}
