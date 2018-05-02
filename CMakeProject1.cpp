// CMakeProject1.cpp : Defines the entry point for the application.
//

#define WHITE		   1,    1,    1
#define BLACK		   0,    0,	   0
#define LIGHT_GREY	0.15, 0.15, 0.15
#define RED			   1,    0,    0

#include "CMakeProject1.h"

using namespace std;

string filename = "C:\\Users\\benny\\source\\tmp\\VtkExample\\data.vtr";
string outputFileName = "human-readable-data.txt";

void ExtratDataToFile(vtkXMLRectilinearGridReader* reader)
{
	ofstream outputFile;

	outputFile.open(outputFileName, ios_base::out);

	for (vtkIdType id = 0; id < reader->GetOutput()->GetNumberOfPoints(); id++)
	{
		double p[3];
		reader->GetOutput()->GetPoint(id, p);
		//std::cout << "Point " << id << " : (" << p[0] << " , " << p[1] << " , " << p[2] << ")" << std::endl;
		outputFile << id << " : (" << p[0] << " , " << p[1] << " , " << p[2] << ")" << std::endl;

		if (id % 10000 == 0)
		{
			cout << id << endl;
		}
	}

	outputFile.close();

	QMessageBox::information(nullptr, "Operation Finished", "Data has been written to file");
}

int main(int argc, char** argv)
{
	QVTKApplication app(argc, argv);
	QMainWindow window;

	// Use QVTKOpenGLWidget instead of QVTKWidget
	QVTKOpenGLWidget* qvtkwidget = new QVTKOpenGLWidget();
	vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
	qvtkwidget->SetRenderWindow(renderWindow.Get());

	vtkNew<vtkXMLRectilinearGridReader> reader;
	if (!reader->CanReadFile(filename.c_str()))
	{
		return 1;
	}
	reader->SetFileName(filename.c_str());
	reader->Update();

	//ExtratDataToFile(reader);

	// Create points

	// Setup scales
	vtkNew<vtkFloatArray> scales;
	scales->SetName("scales");

	/////////////////////////////////////////////////////////////////////////////////////
	vtkNew<vtkRectilinearGridGeometryFilter> plane;
	plane->SetInputConnection(reader->GetOutputPort());

	int* extent = reader->GetOutput()->GetExtent();
	plane->SetExtent(extent[0], extent[1], extent[2], extent[3], extent[4], extent[5]);

	vtkNew<vtkPolyDataMapper> planeMapper;
	planeMapper->SetInputConnection(plane->GetOutputPort());

	vtkNew<vtkActor> planeActor;
	planeActor->SetMapper(planeMapper);
	planeActor->SetScale(1000000);

	vtkNew<vtkRectilinearGridOutlineFilter> outline;
	outline->SetInputConnection(reader->GetOutputPort());

	vtkNew<vtkPolyDataMapper> outlineMapper;
	outlineMapper->SetInputConnection(outline->GetOutputPort());

	vtkNew<vtkActor> outlineActor;
	outlineActor->SetMapper(outlineMapper);
	outlineActor->SetScale(1000000);
	/////////////////////////////////////////////////////////////////////////////////////

	// VTK Renderer
	vtkNew<vtkRenderer> renderer;
	renderer->AddActor(outlineActor.Get());
	renderer->AddActor(planeActor.Get());
	renderer->SetBackground(LIGHT_GREY);

	qvtkwidget->GetRenderWindow()->AddRenderer(renderer.Get());

	window.setFixedSize(1280, 720);
	window.setCentralWidget(qvtkwidget);

	window.show();

	return app.exec();
}
