// CMakeProject1.cpp : Defines the entry point for the application.
//

#define WHITE		   1,    1,    1
#define BLACK		   0,    0,	   0
#define LIGHT_GREY	0.15, 0.15, 0.15
#define RED			   1,    0,    0

#define INIT_TIMER			auto s = std::chrono::high_resolution_clock::now();\
							auto e = std::chrono::high_resolution_clock::now();\
							std::chrono::duration<double> d = e - s

#define MEASURE_SPEED(x)	s = std::chrono::high_resolution_clock::now();\
							x;\
							e = std::chrono::high_resolution_clock::now();\
							d = e - s;\
							std::cout << std::endl;\
							std::cout << std::endl;\
							std::cout << "-------------------------------------------------------------------------" << std::endl;\
							std::cout << "Code: [ " << #x << " ] took " << d.count() << "s to execute" << std::endl;\
							std::cout << "-------------------------------------------------------------------------" << std::endl;\
							std::cout << std::endl;\
							std::cout << std::endl;

#include "CMakeProject1.h"

std::string filename = "C:\\Users\\benny\\source\\tmp\\VtkExample\\data.vtu";

bool OpenReader(vtkXMLUnstructuredGridReader *reader)
{
	if (!reader->CanReadFile(filename.c_str()))
	{
		return false;
	}
	reader->SetFileName(filename.c_str());
	reader->Update();
	return true;
}

int main(int argc, char** argv)
{
	INIT_TIMER; // Init once per scope where you want timer functionality

	QVTKApplication app(argc, argv);
	QMainWindow window;

	// Use QVTKOpenGLWidget instead of QVTKWidget
	QVTKOpenGLWidget* qvtkwidget = new QVTKOpenGLWidget();
	vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
	qvtkwidget->SetRenderWindow(renderWindow.Get());

	vtkNew<vtkXMLUnstructuredGridReader> reader;
	if (!OpenReader(reader))
	{
		std::cin.get();
		return 1;
	}

	double range[2];
	reader->GetOutput()->GetPointData()->GetScalars()->GetRange(range);
	std::cout << range[0] << " - " << range[1] << std::endl;

	// Colors
	MEASURE_SPEED
	(
		vtkNew<vtkLookupTable> colors;
		colors->SetRange(range);
		colors->SetHueRange(2.0 / 3.0, 0); // Or use SetTableValue()
		colors->Build();
	)

	// Surface
	MEASURE_SPEED
	(
		vtkNew<vtkGeometryFilter> unstructuredGeoFilter;
		unstructuredGeoFilter->SetInputData(reader->GetOutput());
		unstructuredGeoFilter->Update();
	)

	MEASURE_SPEED
	(
		vtkNew<vtkPolyDataMapper> surfaceMapper;
		surfaceMapper->SetInputConnection(unstructuredGeoFilter->GetOutputPort());
		surfaceMapper->SetScalarRange(range);
		surfaceMapper->SetLookupTable(colors);
	)

	MEASURE_SPEED
	(
		vtkNew<vtkActor> surfaceActor;
		surfaceActor->SetMapper(surfaceMapper);
	)

	// Isosurfaces
	MEASURE_SPEED
	(
		vtkNew<vtkContourFilter> contour;
		contour->AddInputConnection(reader->GetOutputPort());
		contour->GenerateValues(20, range);
		contour->Update();
	)

	MEASURE_SPEED
	(
		vtkNew<vtkPolyDataMapper> contourMapper;
		contourMapper->SetInputConnection(contour->GetOutputPort());
		contourMapper->SetScalarRange(range);
		contourMapper->SetLookupTable(colors);
	)

	MEASURE_SPEED
	(
		vtkNew<vtkActor> contourActor;
		contourActor->SetMapper(contourMapper);
	)

	// Verticies Actor
	MEASURE_SPEED
	(
		vtkNew<vtkVertexGlyphFilter> verticesGlyph;
		verticesGlyph->AddInputData(reader->GetOutput());
		verticesGlyph->Update();
	)

	MEASURE_SPEED
	(
		vtkNew<vtkPolyDataMapper> vertMapper;
		vertMapper->SetInputConnection(verticesGlyph->GetOutputPort());
		vertMapper->SetScalarRange(range);
		vertMapper->SetLookupTable(colors);
	)

	MEASURE_SPEED
	(
		vtkNew<vtkActor> vertActor;
		vertActor->SetMapper(vertMapper);
		vertActor->GetProperty()->SetPointSize(2);
	)

	// Clipping
	MEASURE_SPEED
	(
		vtkNew<vtkPlane> clippingPlane;
		clippingPlane->SetNormal(1, 0, 0);
		clippingPlane->SetOrigin(0, 0, 0);
	)

	MEASURE_SPEED
	(
		vtkNew<vtkCutter> cutter;
		cutter->SetInputData(reader->GetOutput());
		cutter->SetCutFunction(clippingPlane);
		cutter->Update();
	)

	MEASURE_SPEED
	(
		vtkNew<vtkGeometryFilter> cutterPlane;
		cutterPlane->SetInputData(cutter->GetOutput());
		cutterPlane->Update();
	)

	MEASURE_SPEED
	(
		vtkNew<vtkDataSetMapper> dataMapper;
		dataMapper->SetInputConnection(cutterPlane->GetOutputPort());
		dataMapper->SetScalarRange(range);
		dataMapper->SetLookupTable(colors);
	)
	
	MEASURE_SPEED
	(
		vtkNew<vtkActor> cutActor;
		cutActor->SetMapper(dataMapper);
		cutActor->GetProperty()->SetRepresentationToSurface();
	)

	// Color legend
	MEASURE_SPEED
	(
		vtkNew<vtkScalarBarActor> legendActor;
		legendActor->SetLookupTable(colors);
		legendActor->SetTitle("Color Legend");
		legendActor->SetNumberOfLabels(10);
	)

	vtkNew<vtkRenderer> renderer;

	// Axes
	MEASURE_SPEED
	(
		vtkNew<vtkCubeAxesActor> axes;
		axes->SetCamera(renderer->GetActiveCamera());
		axes->SetBounds(reader->GetOutput()->GetBounds());
		axes->GetTitleTextProperty(0)->SetColor(WHITE);
		axes->GetLabelTextProperty(0)->SetColor(WHITE);
		axes->GetTitleTextProperty(1)->SetColor(WHITE);
		axes->GetLabelTextProperty(1)->SetColor(WHITE);
		axes->GetTitleTextProperty(2)->SetColor(WHITE);
		axes->GetLabelTextProperty(2)->SetColor(WHITE);
		axes->DrawXGridlinesOn();
		axes->DrawYGridlinesOn();
		axes->DrawZGridlinesOn();
		axes->SetGridLineLocation(vtkCubeAxesActor::VTK_GRID_LINES_FURTHEST);
		axes->XAxisMinorTickVisibilityOff();
		axes->YAxisMinorTickVisibilityOff();
		axes->ZAxisMinorTickVisibilityOff();
	)

	// VTK Renderer
	//renderer->AddActor(surfaceActor);
	MEASURE_SPEED
	(
		renderer->AddActor(contourActor);
		renderer->AddActor(vertActor);
		renderer->AddActor(cutActor);
		renderer->AddActor(legendActor);
		renderer->AddActor(axes);
		renderer->SetBackground(LIGHT_GREY);
	)
	

	renderer->ResetCamera(reader->GetOutput()->GetBounds()); // Focus the camera

	qvtkwidget->GetRenderWindow()->AddRenderer(renderer.Get());

	window.setFixedSize(1280, 720);
	window.setCentralWidget(qvtkwidget);

	window.show();

	return app.exec();
}
