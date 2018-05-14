// CMakeProject1.cpp : Defines the entry point for the application.
//

#define WHITE		   1,    1,    1
#define BLACK		   0,    0,	   0
#define LIGHT_GREY	0.15, 0.15, 0.15
#define RED			   1,    0,    0

#include "CMakeProject1.h"

using namespace std;

string filepath = "C:\\Users\\benny\\Desktop\\electrostatics.vtu";
string outputFileName = "human-readable-data.txt";

int main(int argc, char** argv)
{
	QVTKApplication app(argc, argv);
	QMainWindow window;

	// Use QVTKOpenGLWidget instead of QVTKWidget
	QVTKOpenGLWidget* qvtkwidget = new QVTKOpenGLWidget();
	vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
	qvtkwidget->SetRenderWindow(renderWindow.Get());

	vtkNew<vtkXMLUnstructuredGridReader> reader;
	if (!reader->CanReadFile(filepath.c_str()))
	{
		return 1;
	}
	reader->SetFileName(filepath.c_str());
	reader->Update();

	std::cout << reader->GetOutput()->GetPointData()->GetScalars()->GetName() << std::endl;
	std::cout << reader->GetOutput()->GetPointData()->GetVectors()->GetName() << std::endl;

	vtkNew<vtkMaskPoints> mask;
	mask->SetInputConnection(reader->GetOutputPort());
	mask->SetOnRatio(50);

	vtkNew<vtkArrowSource> arrow;
	arrow->SetTipResolution(6);
	arrow->SetTipRadius(0.1);
	arrow->SetTipLength(0.35);
	arrow->SetShaftResolution(6);
	arrow->SetShaftRadius(0.03);
	vtkNew<vtkGlyph3D> glyph;
	glyph->SetInputConnection(mask->GetOutputPort());
	glyph->SetSourceConnection(arrow->GetOutputPort());
	glyph->SetScaleModeToScaleByScalar();
	glyph->ScalingOn();
	glyph->OrientOn();
	glyph->SetScaleFactor(3.6e-7);
	glyph->Update();

	vtkNew<vtkDataSetMapper> mapper;
	mapper->SetInputConnection(glyph->GetOutputPort());

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
