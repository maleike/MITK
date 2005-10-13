
#include "mitkManualSegmentationToSurfaceFilter.h"
#include <itksys/SystemTools.hxx>
#include "mitkDataTreeNodeFactory.h"
#include <mitkSurfaceVtkWriter.h>
#include <vtkSTLWriter.h>

#include <fstream>
/**
* Test class for ManualSegmentationToSurfaceFilter and ImageToSurface
* 1. Read an image
* 2. Create a surface
* 3. Create a Surface with all image processing facillities
*/
int mitkManualSegmentationToSurfaceFilterTest(int argc, char* argv[])
{
  
  if(argc==0)
  {
    std::cout<<"no file specified [FAILED]"<<std::endl;
    return EXIT_FAILURE;
  }
  
  std::string path = argv[1];
  itksys::SystemTools::ConvertToUnixSlashes(path);
  std::string fileIn = path + "/BallBinary30x30x30.pic.gz";
  std::string fileOut = path + "/BallBinary30x30x30.stl";

  
  fileIn = itksys::SystemTools::ConvertToOutputPath(fileIn.c_str());
  fileOut = itksys::SystemTools::ConvertToOutputPath(fileOut.c_str());
  std::cout<<"Eingabe Datei: "<<fileIn<<std::endl;
  std::cout<<"Ausgabe Datei: "<<fileOut<<std::endl;
  mitk::Image::Pointer image = NULL;
  mitk::DataTreeNodeFactory::Pointer factory = mitk::DataTreeNodeFactory::New();
  try
  {
    std::cout << "Loading file: ";
    factory->SetFileName( fileIn.c_str() );
    factory->Update();

    if(factory->GetNumberOfOutputs()<1)
    {
      std::cout<<"file could not be loaded [FAILED]"<<std::endl;
      return EXIT_FAILURE;
    }
    mitk::DataTreeNode::Pointer node = factory->GetOutput( 0 );
    image = dynamic_cast<mitk::Image*>(node->GetData());
    if(image.IsNull())
    {
      std::cout<<"file not an image - test will not be applied [PASSED]"<<std::endl;
      std::cout<<"[TEST DONE]"<<std::endl;
      return EXIT_SUCCESS;
    }
  }
  catch ( itk::ExceptionObject & ex )
  {
    std::cout << "Exception: " << ex << "[FAILED]" << std::endl;
    return EXIT_FAILURE;
  }


  mitk::ManualSegmentationToSurfaceFilter::Pointer filter;
  std::cout << "Instantiat mitk::ManualSegmentationToSurfaceFilter - implicit: mitk::ImageToSurface: ";
  filter = mitk::ManualSegmentationToSurfaceFilter::New();

  if (filter.IsNull())
  {
    std::cout<<"[FAILED]"<<std::endl;
    return EXIT_FAILURE;
  }
  else {
  std::cout<<"[PASSED]"<<std::endl;
  }


  //Wirter instance
  mitk::SurfaceVtkWriter<vtkSTLWriter>::Pointer writer = mitk::SurfaceVtkWriter<vtkSTLWriter>::New();
  if (filter.IsNull())
  {
    std::cout<<"Instantiat SurfaceVtkWirter: [FAILED]"<<std::endl;
    return EXIT_FAILURE;
  }
  else {  
    writer->GlobalWarningDisplayOn();
    writer->SetFileName(fileOut.c_str());
    writer->GetVtkWriter()->SetFileTypeToBinary();  
   
  }
  
  std::cout << "Create surface with default settings: ";
  filter->SetInput(image);
  writer->SetInput(filter->GetOutput());
  writer->Write();

  if( writer->GetNumberOfInputs() < 1 )
  {
     std::cout<<"[FAILED]"<<std::endl;
     writer->Delete();
     return EXIT_FAILURE;
  }
  else
  {
     std::cout<<"[PASSED]"<<std::endl;
  }
 
  std::cout << "Create surface all optimised settings: ";
  //configure ImageToSurfaceFilter
  filter->MedianFilter3DOn();
  filter->SetStandardDeviation(1.5);
  filter->InterpolationOn();
  filter->UseStandardDeviationOn();
  filter->SetThreshold( 1 ); //if( Gauss ) --> TH manipulated for vtkMarchingCube
  filter->DecimateOn();
  filter->SetTargetReduction(0.05f);
  filter->SmoothOn();

  //filter->Update();
  writer->SetInput( filter->GetOutput() );
  if( writer->GetNumberOfInputs() < 1 )
  {
     std::cout<<"[FAILED]"<<std::endl;
     return EXIT_FAILURE;
  }
  else
  {
     std::cout<<"[PASSED]"<<std::endl;
     writer->Write();
  }

  std::cout<<"[TEST DONE]"<<std::endl;
  return EXIT_SUCCESS;
}



