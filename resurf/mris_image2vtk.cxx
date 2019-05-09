#define COMPILING_MRISURF_TOPOLOGY
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "vtkSmartPointer.h"

#include "itkMesh.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkBinaryMask3DMeshSource.h"
#include "itkMeshFileWriter.h"

#include "vtkContourFilter.h"
#include "vtkPolyDataConnectivityFilter.h"
#include "vtkImageExport.h"
#include "itkMesh.h"
#include "itkImageFileWriter.h"
#include "vtkMarchingCubes.h"
#include "vtkImageThreshold.h"
#include "vtkWindowedSincPolyDataFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkTriangleFilter.h"
#include "vtkCleanPolyData.h"
#include "vtkXMLUnstructuredGridWriter.h"
#include "vtkPolyDataWriter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"
#include "vtkImageResample.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkFillHolesFilter.h"
//#include "vtkNIFTIImageReader.h"
#include "vtkDecimatePro.h"

#include "vtkCellArray.h"
#include "vtkDataArray.h"
#include "vtkPointData.h"
#include "macros.h"
#include "vtkImageData.h"
#include "mri.h"
#include "mrisurf.h"
#include "vtkIntArray.h"
#include "vtkSmartPointer.h"
#include "vtkShortArray.h"
#include "vtkLongArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkFloatArray.h"


#include "macros.h"
#include "error.h"
#include "diag.h"
#include "proto.h"
#include "mrisurf.h"
#include "mri.h"
#include "macros.h"
#include "fio.h"
#include "mrishash.h"
#include "cma.h"
#include "version.h"
#include "vtkDelaunay3D.h"
#include "vtkFillHolesFilter.h"
#include "vtkImageDilateErode3D.h"
#include "vtkImageGaussianSmooth.h"
#include "vtkImageContinuousErode3D.h"
#include "vtkImageContinuousDilate3D.h"
#include "vtkPolyDataToImageStencil.h"
#include "vtkImageStencil.h"

vtkSmartPointer<vtkImageData> CreateImage( MRI* rasMRI )
{
  // first copy mri data to image
  vtkDataArray *scalars = NULL;
  vtkUnsignedCharArray  *ucharScalars = NULL;
  vtkIntArray           *intScalars = NULL;
  vtkShortArray         *shortScalars = NULL;
  vtkLongArray          *longScalars = NULL;
  vtkFloatArray         *floatScalars = NULL;
  vtkIdType cValues;

  vtkIdType zX = rasMRI->width;
  vtkIdType zY = rasMRI->height;
  vtkIdType zZ = rasMRI->depth;
  vtkIdType zFrames = rasMRI->nframes;

  vtkSmartPointer<vtkImageData> imageData =vtkSmartPointer<vtkImageData>::New();

  // This object's output space is in voxel coordinates.
	std::cout << zX<< zY<<zZ << std::endl;
  imageData->SetDimensions( zX, zY, zZ );

  double origin[3] = {0,0,0};
/*  if ( m_bResampleToRAS && !m_volumeRef )
  {
    float bounds[6];
    GetBounds( bounds );
    origin[0] = bounds[0];
    origin[1] = bounds[2];
    origin[2] = bounds[4];
  }
  else if ( m_volumeRef )
  {
   double ras[3], cindex[3];

    ::MRIvoxelToWorld( rasMRI, 0., 0., 0., &ras[0], &ras[1], &ras[2] );
    ::MRIworldToVoxel( m_volumeRef->m_MRITarget, ras[0], ras[1], ras[2], &cindex[0], &cindex[1], &cindex[2] );

    m_volumeRef->GetImageOutput()->GetOrigin( origin );
    for ( int i = 0; i < 3; i++ )
    {
      if ( fabs(cindex[i]-nint(cindex[i])) < 1e-4 )
      {
        cindex[i] = nint(cindex[i]);
      }
    }

    origin[0] += cindex[0] * m_volumeRef->m_MRITarget->xsize;
    origin[1] += cindex[1] * m_volumeRef->m_MRITarget->ysize;
    origin[2] += cindex[2] * m_volumeRef->m_MRITarget->zsize;

  //}*/

  imageData->SetSpacing( rasMRI->xsize*1000, rasMRI->ysize*1000, rasMRI->zsize *1000);
  imageData->SetOrigin( origin[0], origin[1], origin[2] );
  //  imageData->SetWholeExtent( 0, zX-1, 0, zY-1, 0, zZ-1 );
  imageData->SetDimensions(zX, zY, zZ);
  if (rasMRI->type == MRI_RGB)
    zFrames = 4;

  // create the scalars for all of the images. set the element size
  // for the data we will read.
#if VTK_MAJOR_VERSION > 5
  switch ( rasMRI->type )
  {
  case MRI_UCHAR:
  case MRI_RGB:
    imageData->AllocateScalars(VTK_UNSIGNED_CHAR, zFrames);
    break;
  case MRI_INT:
    imageData->AllocateScalars(VTK_INT, zFrames);
    break;
  case MRI_LONG:
    imageData->AllocateScalars(VTK_LONG, zFrames);
    break;
  case MRI_FLOAT:
    imageData->AllocateScalars(VTK_FLOAT, zFrames);
    break;
  case MRI_SHORT:
    imageData->AllocateScalars(VTK_SHORT, zFrames);
    break;
  default:
	;  
}
#else
  imageData->SetNumberOfScalarComponents(zFrames);
  switch ( rasMRI->type )
  {
  case MRI_UCHAR:
  case MRI_RGB:
    imageData->SetScalarTypeToUnsignedChar();
    break;
  case MRI_INT:
    imageData->SetScalarTypeToInt();
    break;
  case MRI_LONG:
    imageData->SetScalarTypeToLong();
    break;
  case MRI_FLOAT:
    imageData->SetScalarTypeToFloat();
    break;
  case MRI_SHORT:
    imageData->SetScalarTypeToShort();
    break;
  default:
	;  
}
  imageData->AllocateScalars();
#endif

  return imageData;
}
void CopyMRIDataToImage( MRI* mri, vtkImageData* image )
{
	// Copy the slice data into the scalars.
	int zX = mri->width;
	int zY = mri->height;
	int zZ = mri->depth;
	int zFrames = mri->nframes;

	vtkIdType nTuple = 0;
	vtkDataArray *scalars = image->GetPointData()->GetScalars();
	for ( int nZ = 0; nZ < zZ; nZ++ )
	{
		for ( int nY = 0; nY < zY; nY++ )
		{
			for ( int nX = 0; nX < zX; nX++ )
			{
				for ( int nFrame = 0; nFrame < zFrames; nFrame++ )
				{
					//int hola = mri->slices[nZ+(nFrame)*mri->depth][nY][nX];
					//scalars->SetComponent( nTuple, nFrame,hola);
			
					switch ( mri->type )
					{
						case MRI_UCHAR:
							scalars->SetComponent( nTuple, nFrame,
									MRIseq_vox( mri, nX, nY, nZ, nFrame ) );
							break;
						case MRI_INT:
							scalars->SetComponent( nTuple, nFrame,
									MRIIseq_vox( mri, nX, nY, nZ, nFrame ) );
							break;
						case MRI_LONG:
							scalars->SetComponent( nTuple, nFrame,
									MRILseq_vox( mri, nX, nY, nZ, nFrame ) );
							break;
						case MRI_FLOAT:
							scalars->SetComponent( nTuple, nFrame,
									MRIFseq_vox( mri, nX, nY, nZ, nFrame ) );
							break;
						case MRI_SHORT:
							scalars->SetComponent( nTuple, nFrame,
									MRISseq_vox( mri, nX, nY, nZ, nFrame ) );
							break;
						default:
							break;
					}

				}
				if (mri->type == MRI_RGB)
				{
					int val = MRIIseq_vox(mri, nX, nY, nZ, 0);
					scalars->SetComponent( nTuple, 0, val & 0x00ff);
					scalars->SetComponent( nTuple, 1, (val >> 8) & 0x00ff);
					scalars->SetComponent( nTuple, 2, (val >> 16) & 0x00ff);
					scalars->SetComponent( nTuple, 3, 255);
				}
				nTuple++;
			}
		}

	}
}
int main( int argc, char* argv[] )
{
	if( argc != 7 )
	{
		std::cerr << "Usage: "<< std::endl;
		std::cerr << argv[0];
		std::cerr << " <InputFileName> <OutputFileName> <Lower Threshold> <Upper Threshold> <vtk smoothing iterations> <image smoothing size>";
		std::cerr << std::endl;
		return EXIT_FAILURE;
	}

	const char * inputFileName = argv[1];
	const char * outputFileName = argv[2];

 	MRI *imageFS =  MRIread(inputFileName) ;
	vtkSmartPointer<vtkImageData> vtkImage = CreateImage(imageFS);
	CopyMRIDataToImage(imageFS, vtkImage);	

	using PixelType = unsigned char;

	auto label = static_cast< PixelType >( std::stof( argv[3] ) );
	auto upperThreshold = static_cast< PixelType >( std::stof( argv[4] ) );
	auto vtkSmoothingIterations= static_cast< PixelType >( atoi( argv[5] ) );
	auto imageSmoothingIterations= static_cast< PixelType >( atoi( argv[6] ) );
	//vtkSmartPointer<vtkNIFTIImageReader> reader =  vtkSmartPointer<vtkNIFTIImageReader>::New();
	//reader->SetFileName(inputFileName);
	//reader->Update();

	vtkSmartPointer<vtkImageDilateErode3D> dilateErode =
	vtkSmartPointer<vtkImageDilateErode3D>::New();
	dilateErode->SetInput(vtkImage);
	dilateErode->SetDilateValue(label);
	dilateErode->SetErodeValue(label);
	dilateErode->SetKernelSize(5, 5, 5);
	dilateErode->ReleaseDataFlagOff();
	dilateErode->Update();

	vtkSmartPointer<vtkImageThreshold> threshold = vtkSmartPointer<vtkImageThreshold>::New();
	threshold->SetInput( dilateErode->GetOutput());
	threshold->ThresholdBetween( label-0.5, label+0.5 );
	threshold->ReplaceOutOn();
	threshold->SetOutValue( 0 );
	threshold->Update();
	vtkImage= threshold->GetOutput();
	/*for(int i=0;i<imageSmoothingIterations;i++)
	{
		vtkSmartPointer<vtkImageContinuousErode3D> erode3D = vtkSmartPointer<vtkImageContinuousErode3D>::New();
		erode3D->SetInput(vtkImage);
		erode3D->SetKernelSize(2, 2, 2);

		vtkSmartPointer<vtkImageContinuousDilate3D> dilate3D = vtkSmartPointer<vtkImageContinuousDilate3D>::New();
		dilate3D->SetInputConnection(erode3D->GetOutputPort());
		dilate3D->SetKernelSize(2, 2, 2);
		dilate3D->Update();
		vtkImage = dilate3D->GetOutput();
	}

	vtkSmartPointer<vtkImageGaussianSmooth> gaussianSmoothFilter =    vtkSmartPointer<vtkImageGaussianSmooth>::New();
	gaussianSmoothFilter->SetInput(vtkImage);
	gaussianSmoothFilter->SetRadiusFactors(.2,.2,.2);
	gaussianSmoothFilter->Update();
	vtkImage = gaussianSmoothFilter->GetOutput();
*/
/*	vtkSmartPointer<vtkImageResample> resampler = vtkSmartPointer<vtkImageResample>::New();
	resampler->SetAxisMagnificationFactor(0, 2.0);
	resampler->SetAxisMagnificationFactor(1, 2.0);
	resampler->SetAxisMagnificationFactor(2, 2.0);
	resampler->SetInputConnection(threshold->GetOutputPort());
*/	
	vtkSmartPointer<vtkPolyData> vtkSurface; 
	for(int i =0;i<vtkSmoothingIterations;i++)
	{
		/*vtkSmartPointer<vtkImageGaussianSmooth> gaussianSmoothFilter =    vtkSmartPointer<vtkImageGaussianSmooth>::New();
		gaussianSmoothFilter->SetInput(vtkImage);
		gaussianSmoothFilter->SetRadiusFactors(.2,.2,.2);
		gaussianSmoothFilter->Update();
		vtkImage = gaussianSmoothFilter->GetOutput();*/
		//vtkSmartPointer<vtkMarchingCubes> contour =vtkSmartPointer<vtkMarchingCubes>::New(); 
		
		
		vtkSmartPointer<vtkContourFilter> contour = vtkSmartPointer<vtkContourFilter>::New();
		contour->ComputeGradientsOn();
		contour->SetInput( vtkImage);
		
		//contour->SetValue(0, (i==0)?label-1:1);	
		contour->SetValue(0, label);	
		contour->SetNumberOfContours(1);
		contour->Update();

		vtkSmartPointer<vtkPolyDataConnectivityFilter> conn =	vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
		conn->SetInputConnection( contour->GetOutputPort() );
		conn->SetExtractionModeToLargestRegion();
		vtkSurface = conn->GetOutput();
		/*
		   vtkSmartPointer<vtkFlyingEdges3D> isosurface  = vtk.vtkFlyingEdges3D();
		   isosurface->SetInputConnection(conn->GetOutputPort());
		   isosurface->SetValue(label-0.5, label+0.5);
		   isosurface->ComputeNormalsOn();
		   isosurface->ComputeGradientsOn();
		   isosurface->ComputeScalarsOn();
		   isosurface->InterpolateAttributesOff();
		   isosurface->Update();
		   vtkSurface = isosurface->GetOutput();*/
		vtkSmartPointer<vtkFillHolesFilter> fillHoles =	vtkSmartPointer<vtkFillHolesFilter>::New();
		fillHoles->SetInput(vtkSurface);
		fillHoles->SetHoleSize(10000000000.0);
		fillHoles->Update();
		vtkSurface = fillHoles->GetOutput();

		vtkSmartPointer<vtkDecimatePro> decimate = vtkSmartPointer<vtkDecimatePro>::New();
		decimate->SetInput(vtkSurface);
		//decimate->SetPreserveTopology(true);
		//decimate->SplittingOff();
		decimate->BoundaryVertexDeletionOn();
		//if(i ==0)
			decimate->SetTargetReduction(.9); //99% reduction (if there was 100 triangles, now there will be 1)
		//else
		//	decimate->SetTargetReduction(.01); //99% reduction (if there was 100 triangles, now there will be 1)
		decimate->Update();
		vtkSurface = decimate->GetOutput();

		vtkSmartPointer<vtkTriangleFilter> stripper =		vtkSmartPointer<vtkTriangleFilter>::New();
		stripper->SetInput( vtkSurface );
		stripper->PassVertsOff();
		stripper->PassLinesOff();

		vtkSmartPointer<vtkCleanPolyData> cleaner = vtkSmartPointer<vtkCleanPolyData>::New();
		cleaner->SetAbsoluteTolerance(0.01);
		cleaner->SetInputConnection(stripper->GetOutputPort());
		cleaner->Update();

		vtkSmartPointer<vtkSmoothPolyDataFilter> smoother =     vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
		//smoother->SetInputConnection( contour->GetOutputPort() );
		smoother->SetInputConnection( cleaner->GetOutputPort() );
		smoother->SetNumberOfIterations(50);
		smoother->SetRelaxationFactor(0.3);
		smoother->FeatureEdgeSmoothingOff();
		smoother->BoundarySmoothingOn();
		/*		smoother->SetRelaxationFactor(.7);
				smoother->SetNumberOfIterations(150);
				smoother->SetFeatureAngle(35);
				smoother->FeatureEdgeSmoothingOn();
				smoother->SetFeatureAngle(95);
				smoother->SetEdgeAngle(0.1);
				smoother->SetRelaxationFactor(.7);
				smoother->FeatureEdgeSmoothingOn();
				smoother->BoundarySmoothingOn();
				smoother->SetConvergence(0);
				*/		
		smoother->Update();
		vtkSurface = smoother->GetOutput();

		// polygonal data --> image stencil:
		vtkSmartPointer<vtkPolyDataToImageStencil> pol2stenc =	vtkSmartPointer<vtkPolyDataToImageStencil>::New();
		pol2stenc->SetInput(vtkSurface);
		pol2stenc->SetOutputOrigin(vtkImage->GetOrigin());
		pol2stenc->SetOutputSpacing(vtkImage->GetSpacing());
		pol2stenc->SetOutputWholeExtent(vtkImage->GetExtent());
		pol2stenc->Update();

		// cut the corresponding white image and set the background:
		vtkSmartPointer<vtkImageStencil> imgstenc = vtkSmartPointer<vtkImageStencil>::New();
		imgstenc->SetInput(vtkImage);
		imgstenc->SetStencil(pol2stenc->GetOutput());
		imgstenc->ReverseStencilOff();
		imgstenc->SetBackgroundValue(0);
		imgstenc->Update();
		vtkImage =  imgstenc->GetOutput();
	}









/*






	vtkSmartPointer<vtkImageThreshold> threshold2 = vtkSmartPointer<vtkImageThreshold>::New();
	threshold2->SetInput( vtkImage);
	threshold2->ThresholdBetween(lowerThreshold,upperThreshold);
	threshold2->Update();

	vtkSmartPointer<vtkMarchingCubes> contour = vtkSmartPointer<vtkMarchingCubes>::New();
	contour->SetInput(vtkImage);
	//contour->SetInputConnection(threshold2->GetOutputPort());
	contour->SetValue(0, lowerThreshold+1);

	vtkSmartPointer<vtkPolyDataConnectivityFilter> conn = vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
	conn->SetInputConnection( contour->GetOutputPort() );
	conn->SetExtractionModeToLargestRegion();
	conn->Update();

	vtkSmartPointer<vtkPolyDataNormals> normals =   vtkSmartPointer<vtkPolyDataNormals>::New();
	normals->SetInputConnection( conn->GetOutputPort() );
	normals->ConsistencyOn();
	normals->Update();

	vtkSmartPointer<vtkTriangleFilter> stripper =  vtkSmartPointer<vtkTriangleFilter>::New();
	stripper->SetInputConnection( normals->GetOutputPort() );
	stripper->Update();   

	vtkSmartPointer<vtkCleanPolyData> clean=  vtkSmartPointer<vtkCleanPolyData>::New();
	clean->SetInputConnection( stripper->GetOutputPort() );
	clean->Update();   

	vtkSmartPointer<vtkDecimatePro> decimate = vtkSmartPointer<vtkDecimatePro>::New();
	decimate->SetInput(contour->GetOutput());
	//decimate->SetPreserveTopology(true);
	//decimate->SplittingOff();
	decimate->SetTargetReduction(.95); //99% reduction (if there was 100 triangles, now there will be 1)
	decimate->Update();
  //
  //
	vtkSmartPointer<vtkSmoothPolyDataFilter> smoother2 = vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
	smoother2->SetInputConnection( decimate->GetOutputPort() );
	smoother2->SetFeatureEdgeSmoothing(true);
	smoother2->SetFeatureAngle(.30);
	smoother2->SetNumberOfIterations(100);//smoothingIterations);
	smoother2->Update();
	vtkSmartPointer<vtkSmoothPolyDataFilter> smoothFilter =     vtkSmartPointer<vtkSmoothPolyDataFilter>::New();
	smoothFilter->SetInputConnection(decimate->GetOutputPort());
//	smoothFilter->SetNumberOfIterations(10);
//	smoothFilter->SetFeatureAngle(15);
//	smoothFilter->SetEdgeAngle(15);
//	smoothFilter->SetRelaxationFactor(.9);
//	smoothFilter->FeatureEdgeSmoothingOn();
//	smoothFilter->BoundarySmoothingOn();
	smoothFilter->Update();

	// Update normals on newly smoothed polydata
	vtkSmartPointer<vtkPolyDataNormals> normalGenerator = vtkSmartPointer<vtkPolyDataNormals>::New();
	normalGenerator->SetInputConnection(smoothFilter->GetOutputPort());
	normalGenerator->ComputePointNormalsOn();
	normalGenerator->ComputeCellNormalsOn();
	normalGenerator->Update();

	vtkSmartPointer<vtkPolyData> vtkSurface = smoothFilter->GetOutput();
	//vtkSmartPointer<vtkPolyData> vtkSurface = normalGenerator->GetOutput();
	//vtkSmartPointer<vtkPolyData> vtkSurface = smoother2->GetOutput();
	*/
	
	MRIS* surf = MRISalloc( vtkSurface->GetNumberOfPoints(), vtkSurface->GetNumberOfPolys());
	surf->type = MRIS_TRIANGULAR_SURFACE;

	for(int i=0; i<vtkSurface->GetNumberOfPoints();i++)
	{	
		double* point = vtkSurface->GetPoint( i);
		double* point2 = vtkSurface->GetPoint( i);
		//::MRIvoxelToWorld( imageFS, point[0]/0.013, point[1]/0.1, point[2]/0.013, &point2[0], &point2[1], &point2[2] );
		//::MRIworldToVoxel( m_volumeRef->m_MRITarget, ras[0], ras[1], ras[2], &cindex[0], &cindex[1], &cindex[2] );

		//	MRISsurfaceRASToVoxel(surf, imageFS,point[0], point[1], point[2], &point2[0], &point2[1], &point2[2] );
		
		// MRIvoxelToSurfaceRAS( imageFS,point[0], point[1], point[2], &point2[0], &point2[1], &point2[2] );

		surf->vertices[i].x = (- point2[0]+7.42)/1000;
		surf->vertices[i].z = (- point2[1]+7.5)/1000;
		surf->vertices[i].y = (point2[2]-11.5133)/1000;
		//face = &surf->faces[i];	
	}

	// Copy in the faces.
	vtkIdType cPointIDs = 0;
	vtkIdType* pPointIDs = NULL;
	vtkCellArray* polys = vtkSurface->GetPolys();
	assert( polys );
	vtkIdType nFace = 0;
	for( polys->InitTraversal();polys->GetNextCell( cPointIDs, pPointIDs ); nFace++ ) 
	{
		if( cPointIDs == 3 ) 
		{
			//surf->faces[nFace].v = pPointIDs;
			for( int nPointID = 0; nPointID < 3; nPointID++ )
			{	
				surf->faces[nFace].v[nPointID] = pPointIDs[nPointID];
				///MRIS::faces face = surf->faces[1];
				//face.v[1] = pPointIDs[1];
			}
		}
	}
	//surf->SetMatrix(MRIvoxelXformToRasXform(imageFS));
	
	//surf->vg= imageFS->vg; //=  MRIgetVoxelToRasXform(imageFS) ; //MRIvoxelXformToRasXform(imageFS); //)imageFS->GetMatrix();
	// Write the data.
	MRISwrite( surf, outputFileName);

return EXIT_SUCCESS;
}
	// Write file
/*	vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
	writer->SetFileName(outputFileName);
	writer->SetInputConnection(conn->GetOutputPort());
	writer->Write();
*/
/*	vtkSmartPointer<vtkWindowedSincPolyDataFilter> smoother = vtkSmartPointer<vtkWindowedSincPolyDataFilter>::New();
	smoother->SetInputConnection( stripper->GetOutputPort() );
	smoother->SetNumberOfIterations(500) ; 
//	smoother->FeatureEdgeSmoothingOn();
//	smoother->SetEdgeAngle(30);
	smoother->Update();
*/
//using ReaderType = itk::ImageFileReader< ImageType >;
//ReaderType::Pointer reader = ReaderType::New();
//reader->SetFileName( inputFileName );

/*	typedef itk::SmoothingRecursiveGaussianImageFilter<
	ImageType, ImageType >  SmoothingFilterType;

	SmoothingFilterType::Pointer smoothingRecursiveGaussianImageFilter = SmoothingFilterType::New();
	smoothingRecursiveGaussianImageFilter->SetInput(reader->GetOutput());
	smoothingRecursiveGaussianImageFilter->SetSigma(atoi(argv[6]));
	smoothingRecursiveGaussianImageFilter->Update();

*/
/*using FilterType2 = itk::ImageToVTKImageFilter< ImageType >;
  FilterType2::Pointer filter2 = FilterType2::New();
  filter2->SetInput( smoothingRecursiveGaussianImageFilter->GetOutput() );
//filter2->SetInput( reader->GetOutput() );

filter2->Update();
vtkSmartPointer<vtkImageResample> resampler = vtkSmartPointer<vtkImageResample>::New();
resampler->SetAxisMagnificationFactor(0, 2.0);
resampler->SetAxisMagnificationFactor(1, 2.0);
resampler->SetAxisMagnificationFactor(2, 2.0);
resampler->SetInput(filter2->GetOutput());
resampler->Update();

std::cout << atoi(argv[6]) << std::endl;


vtkSmartPointer<vtkImageGaussianSmooth> gaussianSmoothFilter = 
vtkSmartPointer<vtkImageGaussianSmooth>::New();
gaussianSmoothFilter->SetInputConnection(resampler->GetOutputPort());
gaussianSmoothFilter->SetRadiusFactors(std::stof(argv[6]), std::stof(argv[6]));
gaussianSmoothFilter->Update();
*/

/*
//Get the contour of the PolyData
vtkSmartPointer<vtkContourFilter> contour =
vtkSmartPointer<vtkContourFilter>::New();
contour->SetInput(threshold->GetOutput());
contour->ComputeScalarsOff();
contour->SetValue(0,0.5);
//    
//    //Mesh the largest contour
vtkSmartPointer<vtkPolyDataConnectivityFilter> connectivityFilter =
vtkSmartPointer<vtkPolyDataConnectivityFilter>::New();
connectivityFilter->SetInput(contour->GetOutput());
connectivityFilter->SetExtractionModeToLargestRegion();
connectivityFilter->Update();
//        
//Export vtk Data to ITK
vtkImageExport* exportdata = vtkImageExport::New();
exportdata->SetInput(connectivityFilter->GetOutput());
//            
const unsigned int Dimension = 3;
typedef float PixelType;
typedef itk::Mesh< PixelType, Dimension > MeshType;
typedef itk::ImageFileWriter<MeshType> WriteMeshType;
typedef itk::VTKImageImport<MeshType> importMeshType;
//                
WriteMeshType::Pointer WMeshType = WriteMeshType::New();
importMeshType::Pointer IMeshType = importMeshType::New();
exportdata->SetOutput(IMeshType);
IMeshType->Update();
WMeshType->SetInput(IMeshType->GetOutput());
*/
//typename Surface::Pointer mesh = Self::New();
/*	   vtkSmartPointer<vtkFillHolesFilter> fillHolesFilter =
	   vtkSmartPointer<vtkFillHolesFilter>::New();
	   fillHolesFilter->SetInputData(input);
	   fillHolesFilter->SetHoleSize(1000.0);
	   */

