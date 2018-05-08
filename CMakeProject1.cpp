// CMakeProject1.cpp : Defines the entry point for the application.
//

#define WHITE		   1,    1,    1
#define BLACK		   0,    0,	   0
#define LIGHT_GREY	0.15, 0.15, 0.15
#define RED			   1,    0,    0

#include "CMakeProject1.h"

std::string filename = "C:\\Users\\benny\\source\\tmp\\VtkExample\\data.vtr";

void PrintReaderMetaData(vtkXMLRectilinearGridReader *reader)
{
	std::cout << "Number of components: " << reader->GetOutput()->GetPointData()->GetNumberOfComponents() << std::endl;
	std::cout << "Number of arrays: " << reader->GetOutput()->GetPointData()->GetNumberOfArrays() << std::endl;
	std::cout << "Number of tuples: " << reader->GetOutput()->GetPointData()->GetNumberOfTuples() << std::endl;
	std::cout << "Number of cells: " << reader->GetOutput()->GetNumberOfCells() << std::endl;
}

bool OpenVtrFile(vtkXMLRectilinearGridReader *reader)
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
	QVTKApplication app(argc, argv);
	QMainWindow window;

	// Use QVTKOpenGLWidget instead of QVTKWidget
	QVTKOpenGLWidget* qvtkwidget = new QVTKOpenGLWidget();
	vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
	vtkNew<vtkRenderer> renderer;

	// Reader
	vtkNew<vtkXMLRectilinearGridReader> reader;
	if (!OpenVtrFile(reader)) return 1;


	vtkNew<vtkDoubleArray> scalars;
	scalars->SetName("Mag");
	for (vtkIdType i = 0; i < reader->GetOutput()->GetNumberOfPoints(); i++)
	{
		double p[3];
		reader->GetOutput()->GetPoint(i, p);
		scalars->InsertNextValue(sqrt(pow(p[0], 2) + pow(p[1], 2) + pow(p[2], 2)));
	}

	reader->GetOutput()->GetPointData()->SetScalars(scalars);

	vtkNew<vtkMaskPoints> mask;
	mask->SetInputConnection(reader->GetOutputPort());
	mask->SetOnRatio(1000);

	double bounds[6];
	reader->GetOutput()->GetBounds(bounds);

	vtkNew<vtkLineSource> rake;
	rake->SetPoint1(bounds[0], bounds[2], bounds[4]);
	rake->SetPoint2(bounds[1], bounds[3], bounds[5]);
	rake->SetResolution(21);

	vtkNew<vtkPolyDataMapper> rakeMapper;
	rakeMapper->SetInputConnection(rake->GetOutputPort());

	vtkNew<vtkActor> rakeActor;
	rakeActor->SetMapper(rakeMapper);

	vtkNew<vtkRungeKutta4> integ;
	vtkNew<vtkStreamTracer> sl;
	sl->SetInputConnection(reader->GetOutputPort());
	sl->SetSourceConnection(rake->GetOutputPort());
	sl->SetIntegrator(integ);
	sl->SetMaximumPropagation(0.1);
	sl->SetInitialIntegrationStep(0.2);
	sl->SetIntegrationDirectionToBoth();
	sl->SetMaximumIntegrationStep(vtkStreamTracer::CELL_LENGTH_UNIT);
	sl->SetIntegrationStepUnit(vtkStreamTracer::CELL_LENGTH_UNIT);

	//vtkNew<vtkRuledSurfaceFilter> scalarSurface;
	//scalarSurface->SetInputConnection(sl->GetOutputPort());
	//scalarSurface->SetOffset(0);
	//scalarSurface->SetOnRatio(2);
	//scalarSurface->PassLinesOn();
	//scalarSurface->SetRuledModeToPointWalk();
	//scalarSurface->SetDistanceFactor(30);

	vtkNew<vtkPolyDataMapper> streamLineMapper;
	streamLineMapper->SetInputConnection(sl->GetOutputPort());
	streamLineMapper->SetScalarRange(reader->GetOutput()->GetScalarRange());

	vtkNew<vtkActor> streamLineActor;
	streamLineActor->SetMapper(streamLineMapper);

	vtkNew<vtkRectilinearGridGeometryFilter> geoFilter;
	geoFilter->SetInputConnection(reader->GetOutputPort());
	
	vtkNew<vtkDataSetMapper> vertexMapper;
	vertexMapper->SetInputConnection(geoFilter->GetOutputPort());

	vtkNew<vtkActor> vertexActor;
	vertexActor->SetMapper(vertexMapper);
	vertexActor->GetProperty()->SetPointSize(2);

	vtkNew<vtkRectilinearGridOutlineFilter> outline;
	outline->SetInputConnection(reader->GetOutputPort());

	vtkNew<vtkPolyDataMapper> outlineMapper;
	outlineMapper->SetInputConnection(outline->GetOutputPort());

	vtkNew<vtkActor> outlineActor;
	outlineActor->SetMapper(outlineMapper);

	vtkNew<vtkCubeAxesActor> axesActor;
	axesActor->SetCamera(renderer->GetActiveCamera());
	axesActor->SetBounds(reader->GetOutput()->GetBounds());
	axesActor->GetTitleTextProperty(0)->SetColor(WHITE);
	axesActor->GetLabelTextProperty(0)->SetColor(WHITE);
	axesActor->GetTitleTextProperty(1)->SetColor(WHITE);
	axesActor->GetLabelTextProperty(1)->SetColor(WHITE);
	axesActor->GetTitleTextProperty(2)->SetColor(WHITE);
	axesActor->GetLabelTextProperty(2)->SetColor(WHITE);
	axesActor->DrawXGridlinesOn();
	axesActor->DrawYGridlinesOn();
	axesActor->DrawZGridlinesOn();
	axesActor->SetGridLineLocation(vtkCubeAxesActor::VTK_GRID_LINES_FURTHEST);
	axesActor->XAxisMinorTickVisibilityOff();
	axesActor->YAxisMinorTickVisibilityOff();
	axesActor->ZAxisMinorTickVisibilityOff();

	// VTK Renderer
	renderer->AddActor(outlineActor);
	renderer->AddActor(axesActor);
	renderer->AddActor(rakeActor);
	renderer->AddActor(streamLineActor);
	renderer->SetBackground(LIGHT_GREY);

	renderer->ResetCamera(reader->GetOutput()->GetBounds());

	renderWindow->AddRenderer(renderer);
	qvtkwidget->SetRenderWindow(renderWindow);

	window.setFixedSize(1280, 720);
	window.setCentralWidget(qvtkwidget);
	window.show();

	return app.exec();
}
