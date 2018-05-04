// CMakeProject1.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <math.h>

#include <QVTKApplication.h>
#include <QObject>
#include <QMainWindow>
#include <QApplication>
#include <QMessageBox>

// TODO: Reference additional headers your program requires here.
#include <vtkRenderer.h>
#include <vtkNew.h>
#include <QVTKOpenGLWidget.h>
#include <vtkGenericOpenGLRenderWindow.h>

#include <vtkMaskPoints.h>
#include <vtkCellData.h>
#include <vtkDataArray.h>
#include <vtkFloatArray.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkDataSetMapper.h>
#include <vtkXMLRectilinearGridReader.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkProperty.h>
#include <vtkUnstructuredGrid.h>
#include <vtkRectilinearGrid.h>
#include <vtkRectilinearGridGeometryFilter.h>
#include <vtkRectilinearGridOutlineFilter.h>

#include <vtkLookupTable.h>
#include <vtkUnsignedCharArray.h>

#include <vtkGlyph3D.h>
#include <vtkArrowSource.h>