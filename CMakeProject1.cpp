// CMakeProject1.cpp : Defines the entry point for the application.
//

#define WHITE		   1,    1,    1
#define BLACK		   0,    0,	   0
#define LIGHT_GREY	0.15, 0.15, 0.15
#define RED			   1,    0,    0

#include "CMakeProject1.h"

std::string filename = "C:\\Users\\benny\\source\\tmp\\VtkExample\\data.vtr";
std::string outputFileName = "human-readable-data.txt";

void ExtratDataToFile(vtkXMLRectilinearGridReader* reader)
{
	ofstream outputFile;

	outputFile.open(outputFileName, std::ios_base::out);

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

void PrintReaderMetaData(vtkXMLRectilinearGridReader* reader)
{
	std::cout << "Number of components: " << reader->GetOutput()->GetPointData()->GetNumberOfComponents() << std::endl;
	std::cout << "Number of arrays: " << reader->GetOutput()->GetPointData()->GetNumberOfArrays() << std::endl;
	std::cout << "Number of tuples: " << reader->GetOutput()->GetPointData()->GetNumberOfTuples() << std::endl;
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

	vtkNew<vtkPoints> points;
	reader->GetOutput()->GetPoints(points);

	vtkNew<vtkPolyData> pointsPolyData;
	pointsPolyData->SetPoints(points);

	//ExtratDataToFile(reader);
	PrintReaderMetaData(reader);

	vtkNew<vtkVertexGlyphFilter> vertexFilter;
	vertexFilter->SetInputData(pointsPolyData);
	vertexFilter->Update();

	vtkNew<vtkPolyData> filterOutput;
	filterOutput->ShallowCopy(vertexFilter->GetOutput());

	vtkNew<vtkScalarsToColors> colorLookupTable;
	colorLookupTable->SetVectorModeToMagnitude();
	colorLookupTable->SetRange(0, 255);
	colorLookupTable->Build();

	vtkNew<vtkPolyDataMapper> vertexMapper;
	vertexMapper->SetInputData(filterOutput);
	vertexMapper->SetLookupTable(colorLookupTable);

	vtkNew<vtkActor> vertexActor;
	vertexActor->SetMapper(vertexMapper);
	vertexActor->SetScale(1000000);

	vtkNew<vtkRectilinearGridOutlineFilter> outline;
	outline->SetInputConnection(reader->GetOutputPort());

	vtkNew<vtkPolyDataMapper> outlineMapper;
	outlineMapper->SetInputConnection(outline->GetOutputPort());

	vtkNew<vtkActor> outlineActor;
	outlineActor->SetMapper(outlineMapper);
	outlineActor->SetScale(1000000);

	// VTK Renderer
	vtkNew<vtkRenderer> renderer;
	renderer->AddActor(outlineActor.Get());
	renderer->AddActor(vertexActor.Get());
	renderer->SetBackground(LIGHT_GREY);

	qvtkwidget->GetRenderWindow()->AddRenderer(renderer.Get());

	window.setFixedSize(1280, 720);
	window.setCentralWidget(qvtkwidget);

	window.show();

	return app.exec();
}
