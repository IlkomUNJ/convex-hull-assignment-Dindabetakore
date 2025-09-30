#ifndef DRAWINGCANVAS_H
#define DRAWINGCANVAS_H

#include <QWidget>
#include <QPoint>
#include <QVector>
#include <QPainter>
#include <QMouseEvent>
#include <QtMath>
#include <algorithm>
#include <set>

class DrawingCanvas : public QWidget
{
    Q_OBJECT

public:
    DrawingCanvas(QWidget *parent = nullptr);

    enum AlgorithmType {
        SlowHull,
        FastHull,
        JarvisMarch
    };
    Q_ENUM(AlgorithmType)

    void computeConvexHull(AlgorithmType type);
    void clearCanvas();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    struct QPointLess {
        bool operator()(const QPoint& a, const QPoint& b) const {
            if (a.x() != b.x()) {
                return a.x() < b.x();
            }
            return a.y() < b.y();
        }
    };

    QVector<QPoint> m_points;
    QVector<QPoint> m_hull;
    int m_iterations = 0;
    QString m_algorithmName = "";

    static int orientation(QPoint p, QPoint q, QPoint r);
    static qreal distSq(QPoint p1, QPoint p2);

    void runBruteForce();
    void runGrahamScan();
    void runJarvisMarch();

    static QPoint p0;
    static bool comparePoints(const QPoint& a, const QPoint& b);
};

#endif
