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

void GenerateColors(vtkRectilinearGrid *grid)
{
	// Create color map
	std::unique_ptr<std::vector<double>> mags = std::make_unique<std::vector<double>>();
	double min = 0;
	double max = 0;

	std::cout << "Calculating magnitudes on " << grid->GetNumberOfPoints() << " points ..." << std::endl;
	for (vtkIdType i = 0; i < grid->GetNumberOfPoints(); i++)
	{
		double point[3];
		grid->GetPoint(i, point);
		point[0] = point[0];
		point[1] = point[0];
		point[2] = point[0];
		double tmp = sqrt(pow(point[0], 2) + pow(point[1], 2) + pow(point[2], 2));

		if (i == 0) min = max = tmp;
		if (tmp > max) max = tmp;
		if (tmp < min) min = tmp;

		mags->push_back(tmp);
	}
	std::cout << std::endl;
	std::cout << "Done calculating magnitudes" << std::endl << std::endl;
	std::cout << "Range is " << min << " to " << max << std::endl << std::endl;

	std::cout << "Calculating colors ..." << std::endl;
	vtkNew<vtkLookupTable> lut;
	lut->SetRange(min, max);
	lut->SetHueRange((double)2 / 3, 0);
	lut->Build();

	vtkNew<vtkUnsignedCharArray> colors;
	colors->SetNumberOfComponents(3);
	colors->SetName("Colors");

	for (vtkIdType i = 0; i < grid->GetNumberOfPoints(); i++)
	{
		double dcolor[3];
		lut->GetColor(mags->at(i), dcolor);

		unsigned char color[3];
		for (unsigned int j = 0; j < 3; j++)
		{
			color[j] = (unsigned int)(dcolor[j] * 255.0);
		}

		colors->InsertNextTypedTuple(color);
	}
	std::cout << std::endl;
	std::cout << "Done calculating colors" << std::endl << std::endl;

	grid->GetPointData()->SetScalars(colors);
}

vtkActor *CreateOutline(vtkXMLRectilinearGridReader *reader)
{
	// Outline
	vtkNew<vtkRectilinearGridOutlineFilter> outline;
	outline->SetInputConnection(reader->GetOutputPort());

	vtkNew<vtkPolyDataMapper> outlineMapper;
	outlineMapper->SetInputConnection(outline->GetOutputPort());

	vtkActor *outlineActor = vtkActor::New();
	outlineActor->SetMapper(outlineMapper);
	outlineActor->SetScale(1000000);

	return outlineActor;
}

vtkActor *CreateVertices(vtkXMLRectilinearGridReader *reader)
{
	// Vertices
	//vtkNew<vtkVertexGlyphFilter> vertexFilter;
	//vertexFilter->SetInputConnection(reader->GetOutputPort());
	//vertexFilter->Update();

	vtkNew<vtkDataSetMapper> vertexMapper;
	vertexMapper->SetInputConnection(reader->GetOutputPort());

	vtkActor *vertexActor = vtkActor::New();
	vertexActor->SetMapper(vertexMapper);
	vertexActor->SetScale(1000000);
	vertexActor->GetProperty()->SetPointSize(2);

	return vertexActor;
}

int main(int argc, char** argv)
{
	QVTKApplication app(argc, argv);
	QMainWindow window;

	// Use QVTKOpenGLWidget instead of QVTKWidget
	QVTKOpenGLWidget* qvtkwidget = new QVTKOpenGLWidget();

	// Reader
	vtkNew<vtkXMLRectilinearGridReader> reader;
	if (!OpenVtrFile(reader)) return 1;

	//ExtratDataToFile(reader);
	PrintReaderMetaData(reader);

	GenerateColors(reader->GetOutput());

	vtkNew<vtkMaskPoints> mask;
	mask->SetOnRatio(150);
	mask->SetInputConnection(reader->GetOutputPort());
	mask->Update();

	std::cout << "Starting to create glyphs ..." << std::endl;
	vtkNew<vtkArrowSource> arrow;
	arrow->Update();
	vtkNew<vtkGlyph3D> glyph;
	glyph->SetSourceConnection(arrow->GetOutputPort());
	glyph->SetInputConnection(mask->GetOutputPort());
	glyph->OrientOn();
	glyph->ScalingOff();
	glyph->Update();
	std::cout << "Finished creating glyphs" << std::endl;

	vtkNew<vtkPolyDataMapper> glyphMapper;
	glyphMapper->SetInputConnection(glyph->GetOutputPort());

	vtkNew<vtkActor> glyphActor;
	glyphActor->SetMapper(glyphMapper);

	vtkSmartPointer<vtkActor> vertexActor = CreateVertices(reader);
	vtkSmartPointer<vtkActor> outlineActor = CreateOutline(reader);

	// VTK Renderer
	vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
	vtkNew<vtkRenderer> renderer;
	//renderer->AddActor(vertexActor);
	renderer->AddActor(outlineActor);
	renderer->AddActor(glyphActor);
	renderer->SetBackground(LIGHT_GREY);
	renderWindow->AddRenderer(renderer);

	qvtkwidget->SetRenderWindow(renderWindow);

	window.setFixedSize(1280, 720);
	window.setCentralWidget(qvtkwidget);
	window.show();

	return app.exec();
}
