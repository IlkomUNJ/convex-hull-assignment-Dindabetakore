#include "drawingwindow.h"

DrawingWindow::DrawingWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Convex Hull Algorithm Comparison");

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainVLayout = new QVBoxLayout(centralWidget);

    m_canvas = new DrawingCanvas(this);
    mainVLayout->addWidget(m_canvas);

    QHBoxLayout *buttonHLayout = new QHBoxLayout();

    m_slowButton = new QPushButton("Slow Hull");
    m_fastButton = new QPushButton("Fast Hull");
    m_clearButton = new QPushButton("Clear");

    buttonHLayout->addWidget(m_slowButton);
    buttonHLayout->addWidget(m_fastButton);
    buttonHLayout->addStretch(1);
    buttonHLayout->addWidget(m_clearButton);

    mainVLayout->addLayout(buttonHLayout);

    connect(m_slowButton, &QPushButton::clicked, this, &DrawingWindow::runSlowSimulation);
    connect(m_fastButton, &QPushButton::clicked, this, &DrawingWindow::runFastSimulation);
    connect(m_clearButton, &QPushButton::clicked, this, &DrawingWindow::clearCanvas);

    resize(800, 700);
}

DrawingWindow::~DrawingWindow()
{
}

void DrawingWindow::runSlowSimulation()
{
    m_canvas->computeConvexHull(DrawingCanvas::SlowHull);
}

void DrawingWindow::runFastSimulation()
{
    m_canvas->computeConvexHull(DrawingCanvas::FastHull);
}

void DrawingWindow::clearCanvas()
{
    m_canvas->clearCanvas();
}
