// CMakeProject1.cpp : Defines the entry point for the application.
//

#define WHITE		   1,    1,    1
#define BLACK		   0,    0,	   0
#define LIGHT_GREY	0.15, 0.15, 0.15
#define RED			   1,    0,    0

#include "CMakeProject1.h"

std::string filename = "C:\\Users\\benny\\source\\tmp\\VtkExample\\bcit_temperature.vtu";
std::string outputFileName = "human-readable-data.txt";

void ExtratDataToFile(vtkXMLUnstructuredGridReader* reader)
{
	ofstream outputFile;

	outputFile.open(outputFileName, std::ios_base::out);

	for (vtkIdType id = 0; id < reader->GetOutput()->GetNumberOfPoints(); id++)
	{
		double p[3];
		reader->GetOutput()->GetPoint(id, p);
		outputFile << id << " : (" << p[0] << " , " << p[1] << " , " << p[2] << ")" << std::endl;

		if (id % 10000 == 0)
		{
			std::cout << id << std::endl;
		}
	}

	outputFile.close();

	QMessageBox::information(nullptr, "Operation Finished", "Data has been written to file");
}

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

void ColorData(vtkUnstructuredGrid *uGrid)
{
	double tmp[1];

	vtkNew<vtkLookupTable> lut;

	vtkDataArray *tda = uGrid->GetPointData()->GetScalars();

	double range[2];
	tda->GetRange(range);

	lut->SetTableRange(range[0], range[1]);
	//lut->SetHueRange((double)2/3, 0); // Reversing Hue Range
	lut->SetNumberOfTableValues(9);
	lut->Build();
	// Custom Colors
	lut->SetTableValue(0, 0.00, 0.00, 1.00);
	lut->SetTableValue(1, 0.00, 0.50, 1.00);
	lut->SetTableValue(2, 0.00, 1.00, 1.00);
	lut->SetTableValue(3, 0.00, 1.00, 0.50);
	lut->SetTableValue(4, 0.00, 1.00, 0.00);
	lut->SetTableValue(5, 0.50, 1.00, 0.00);
	lut->SetTableValue(6, 1.00, 1.00, 0.00);
	lut->SetTableValue(7, 1.00, 0.50, 0.00);
	lut->SetTableValue(8, 1.00, 0.00, 0.00);

	vtkNew<vtkUnsignedCharArray> colors;
	colors->SetName("Colors");
	colors->SetNumberOfComponents(3);

	for (vtkIdType i = 0; i < uGrid->GetNumberOfPoints(); i++)
	{
		double dcolor[3];
		unsigned char color[3];

		tda->GetTuple(i, tmp);
		lut->GetColor(*tmp, dcolor);

		for (int j = 0; j < 3; j++)
		{
			color[j] = static_cast<unsigned char>(255.0 * dcolor[j]);
		}

		colors->InsertNextTypedTuple(color);
	}

	uGrid->GetPointData()->SetScalars(colors);
}

int main(int argc, char** argv)
{
	QVTKApplication app(argc, argv);
	QMainWindow window;

	// Use QVTKOpenGLWidget instead of QVTKWidget
	QVTKOpenGLWidget* qvtkwidget = new QVTKOpenGLWidget();
	vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
	qvtkwidget->SetRenderWindow(renderWindow.Get());

	vtkNew<vtkXMLUnstructuredGridReader> reader;
	if (!OpenReader(reader)) return 1;

	//ExtratDataToFile(reader);

	vtkNew<vtkGeometryFilter> unstructuredGeoFilter;
	unstructuredGeoFilter->SetInputData(reader->GetOutput());
	unstructuredGeoFilter->Update();

	vtkNew<vtkPolyDataMapper> surfaceMapper;
	surfaceMapper->SetInputConnection(unstructuredGeoFilter->GetOutputPort());

	vtkNew<vtkActor> surfaceActor;
	surfaceActor->SetMapper(surfaceMapper);

	// Isosurfaces
	double range[2];
	reader->GetOutput()->GetPointData()->GetScalars()->GetRange(range);

	std::cout << range[0] << " - " << range[1] << std::endl;
	
	vtkNew<vtkContourFilter> contour;
	contour->AddInputConnection(reader->GetOutputPort());
	contour->GenerateValues(10, range);
	contour->Update();

	vtkNew<vtkPolyDataMapper> contourMapper;
	contourMapper->SetInputConnection(contour->GetOutputPort());

	vtkNew<vtkActor> contourActor;
	contourActor->SetMapper(contourMapper);

	ColorData(reader->GetOutput());


	// Verticies Actor
	vtkNew<vtkVertexGlyphFilter> verticesGlyph;
	verticesGlyph->AddInputData(reader->GetOutput());
	verticesGlyph->Update();

	vtkNew<vtkPolyDataMapper> vertMapper;
	vertMapper->SetInputConnection(verticesGlyph->GetOutputPort());

	vtkNew<vtkActor> vertActor;
	vertActor->SetMapper(vertMapper);
	vertActor->GetProperty()->SetPointSize(2);


	// Clipping
	vtkNew<vtkPlane> clippingPlane;
	clippingPlane->SetNormal(1, 0, 0);
	clippingPlane->SetOrigin(0, 0, 0);

	vtkNew<vtkCutter> cutter;
	cutter->SetInputData(reader->GetOutput());
	cutter->SetCutFunction(clippingPlane);
	cutter->Update();

	vtkNew<vtkGeometryFilter> cutterPlane;
	cutterPlane->SetInputData(cutter->GetOutput());
	cutterPlane->Update();

	vtkNew<vtkDataSetMapper> dataMapper;
	dataMapper->SetInputConnection(cutterPlane->GetOutputPort());

	vtkNew<vtkActor> cutActor;
	cutActor->SetMapper(dataMapper);
	cutActor->GetProperty()->SetRepresentationToSurface();


	// VTK Renderer
	vtkNew<vtkRenderer> renderer;
	//renderer->AddActor(surfaceActor);
	renderer->AddActor(contourActor);
	renderer->AddActor(vertActor);
	renderer->AddActor(cutActor);
	renderer->SetBackground(LIGHT_GREY);

	qvtkwidget->GetRenderWindow()->AddRenderer(renderer.Get());

	window.setFixedSize(1280, 720);
	window.setCentralWidget(qvtkwidget);

	window.show();

	return app.exec();
}
