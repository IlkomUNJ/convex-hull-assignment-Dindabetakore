#ifndef DRAWINGWINDOW_H
#define DRAWINGWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "drawingcanvas.h"

class DrawingWindow : public QMainWindow
{
    Q_OBJECT

public:
    DrawingWindow(QWidget *parent = nullptr);
    ~DrawingWindow();

private slots:
    void runSlowSimulation();
    void runFastSimulation();
    void clearCanvas();

private:
    DrawingCanvas *m_canvas;

    QPushButton *m_slowButton;
    QPushButton *m_fastButton;
    QPushButton *m_clearButton;
};

#endif // DRAWINGWINDOW_H
