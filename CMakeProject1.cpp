// CMakeProject1.cpp : Defines the entry point for the application.
//

#define WHITE		   1,    1,    1
#define BLACK		   0,    0,	   0
#define LIGHT_GREY	0.15, 0.15, 0.15
#define RED			   1,    0,    0

#include "CMakeProject1.h"

using namespace std;

string rectFile = "C:\\Users\\benny\\source\\tmp\\VtkExample\\data.vtr";
string unstructFile = "C:\\Users\\benny\\source\\tmp\\VtkExample\\data.vtu";
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

	vtkNew<vtkXMLUnstructuredGridReader> rectReader;
	//vtkNew<vtkXMLRectilinearGridReader> rectReader;
	if (!rectReader->CanReadFile(unstructFile.c_str()))
	{
		return 1;
	}
	rectReader->SetFileName(unstructFile.c_str());
	rectReader->Update();

	vtkNew<vtkDataSetMapper> mapper;
	mapper->SetInputConnection(rectReader->GetOutputPort());

	vtkNew<vtkActor> actor;
	actor->SetMapper(mapper);

	// VTK Renderer
	vtkNew<vtkRenderer> renderer;
	renderer->AddActor(actor);
	renderer->SetBackground(LIGHT_GREY);

	qvtkwidget->GetRenderWindow()->AddRenderer(renderer.Get());

	window.setFixedSize(1280, 720);
	window.setCentralWidget(qvtkwidget);

	window.show();

	return app.exec();
}
