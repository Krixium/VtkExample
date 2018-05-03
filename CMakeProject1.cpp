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

void PrintReaderMetaData(vtkXMLRectilinearGridReader *reader)
{
	std::cout << "Number of components: " << reader->GetOutput()->GetPointData()->GetNumberOfComponents() << std::endl;
	std::cout << "Number of arrays: " << reader->GetOutput()->GetPointData()->GetNumberOfArrays() << std::endl;
	std::cout << "Number of tuples: " << reader->GetOutput()->GetPointData()->GetNumberOfTuples() << std::endl;
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

void GenerateColors(vtkPolyData *data)
{
	// Create color map
	std::cout << "Allocating memory for " << data->GetNumberOfPoints() << " magnitudes ..." << std::endl;
	std::unique_ptr<std::vector<double>> mags = std::make_unique<std::vector<double>>();
	double min = 0;
	double max = 0;
	std::cout << "Done allocating memory" << std::endl << std::endl;

	std::cout << "Calculating magnitudes on " << data->GetNumberOfPoints() << " points ..." << std::endl;
	for (vtkIdType i = 0; i < data->GetNumberOfPoints(); i++)
	{
		double point[3];
		data->GetPoint(i, point);
		point[0] = point[0] * 1000000;
		point[1] = point[0] * 1000000;
		point[2] = point[0] * 1000000;
		int tmp = sqrt(pow(point[0], 2) + pow(point[1], 2) + pow(point[2], 2));

		if (i == 0)
		{
			min = max = tmp;
		}

		if (tmp > max)
		{
			max = tmp;
		}
		
		if (tmp < min)
		{
			min = tmp;
		}

		mags->push_back(tmp);
	}
	std::cout << std::endl;
	std::cout << "Done calculating magnitudes" << std::endl << std::endl;
	std::cout << "Range is " << min << " to " << max << std::endl << std::endl;
	
	std::cout << "Calculating colors ..." << std::endl;
	vtkNew<vtkLookupTable> lut;
	lut->SetRange(min, max);
	lut->Build();

	vtkNew<vtkUnsignedCharArray> colors;
	colors->SetNumberOfComponents(3);
	colors->SetName("Colors");

	for (vtkIdType i = 0; i < data->GetNumberOfPoints(); i++)
	{
		double dcolor[3];
		lut->GetColor(mags->at(i), dcolor);

		unsigned char color[3];
		for (unsigned int j = 0; j < 3; j++)
		{
			color[j] = static_cast<unsigned int>(dcolor[j] * 255.0);
		}

		colors->InsertNextTypedTuple(color);
	}
	std::cout << std::endl;
	std::cout << "Done calculating colors" << std::endl << std::endl;

	data->GetPointData()->SetScalars(colors);
}

int main(int argc, char** argv)
{
	QVTKApplication app(argc, argv);
	QMainWindow window;

	// Use QVTKOpenGLWidget instead of QVTKWidget
	QVTKOpenGLWidget* qvtkwidget = new QVTKOpenGLWidget();
	vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
	qvtkwidget->SetRenderWindow(renderWindow.Get());

	// Reader
	vtkNew<vtkXMLRectilinearGridReader> reader;
	if (!OpenVtrFile(reader)) return 1;

	//ExtratDataToFile(reader);
	PrintReaderMetaData(reader);

	// Vertices
	vtkNew<vtkPoints> points;
	reader->GetOutput()->GetPoints(points);

	vtkNew<vtkPolyData> pointsPolyData;
	pointsPolyData->SetPoints(points);

	vtkNew<vtkVertexGlyphFilter> vertexFilter;
	vertexFilter->SetInputData(pointsPolyData);
	vertexFilter->Update();

	vtkNew<vtkPolyData> filterOutput;
	filterOutput->ShallowCopy(vertexFilter->GetOutput());

	GenerateColors(filterOutput);

	vtkNew<vtkPolyDataMapper> vertexMapper;
	vertexMapper->SetInputData(filterOutput);

	vtkNew<vtkActor> vertexActor;
	vertexActor->SetMapper(vertexMapper);
	vertexActor->SetScale(1000000);
	vertexActor->GetProperty()->SetPointSize(2);

	// Outline
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
