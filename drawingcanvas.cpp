#include "drawingcanvas.h"
#include <QPen>
#include <QBrush>
#include <QDebug>

QPoint DrawingCanvas::p0 = QPoint();

DrawingCanvas::DrawingCanvas(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(600, 600);
    setFocusPolicy(Qt::StrongFocus);
    setStyleSheet("background-color: #0088cc;");
}

int DrawingCanvas::orientation(QPoint p, QPoint q, QPoint r)
{
    qreal val = (q.y() - p.y()) * (r.x() - q.x()) -
                (q.x() - p.x()) * (r.y() - q.y());

    if (qFuzzyIsNull(val)) return 0;
    return (val > 0) ? 2 : 1;
}

qreal DrawingCanvas::distSq(QPoint p1, QPoint p2)
{
    return (p1.x() - p2.x()) * (p1.x() - p2.x()) +
           (p1.y() - p2.y()) * (p1.y() - p2.y());
}

bool DrawingCanvas::comparePoints(const QPoint& a, const QPoint& b)
{
    int o = orientation(p0, a, b);
    if (o == 0)
        return (distSq(p0, a) < distSq(p0, b));

    return (o == 2);
}

void DrawingCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_points.append(event->pos());
        m_hull.clear();
        m_iterations = 0;
        m_algorithmName = "";
        update();
    }
}

void DrawingCanvas::clearCanvas()
{
    m_points.clear();
    m_hull.clear();
    m_iterations = 0;
    m_algorithmName = "";
    update();
}

void DrawingCanvas::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pointPen(Qt::blue);
    pointPen.setWidth(5);
    painter.setPen(pointPen);
    painter.setBrush(Qt::SolidPattern);

    for (const QPoint& p : m_points) {
        painter.drawEllipse(p, 3, 3);
    }

    if (m_hull.size() > 1) {
        QPen hullPen(Qt::red);
        hullPen.setWidth(2);
        painter.setPen(hullPen);
        painter.setBrush(Qt::NoBrush);
        painter.drawPolygon(m_hull);
    }

    if (m_points.size() > 0) {
        painter.setPen(Qt::yellow);
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        QString info = QString("Algorithm: %1\n Iterations: %2\n Total Points (n): %3\n Hull Points (h): %4")
                           .arg(m_algorithmName)
                           .arg(m_iterations)
                           .arg(m_points.size())
                           .arg(m_hull.size());

        painter.drawText(10, 20, info);
    }
}

void DrawingCanvas::computeConvexHull(AlgorithmType type)
{
    if (m_points.size() < 2) {
        m_hull = m_points;
        m_iterations = 0;
        m_algorithmName = "";
        update();
        return;
    }

    switch (type) {
    case SlowHull:
        runBruteForce();
        break;
    case FastHull:
        runGrahamScan();
        break;
    case JarvisMarch:
        runJarvisMarch();
        break;
    }
    update();
}

void DrawingCanvas::runBruteForce()
{
    m_hull.clear();
    m_iterations = 0;
    m_algorithmName = "Slow Hull (Brute Force O(n³))";

    int n = m_points.size();
    if (n < 3) {
        m_hull = m_points;
        return;
    }

    std::set<QPoint, QPointLess> hullSet;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) continue;

            bool allOnOneSide = true;
            int refOrientation = 0;

            for (int k = 0; k < n; k++) {
                m_iterations++;
                if (k == i || k == j) continue;

                int o = orientation(m_points[i], m_points[j], m_points[k]);

                if (refOrientation == 0 && o != 0) {
                    refOrientation = o;
                } else if (o != 0 && o != refOrientation) {
                    allOnOneSide = false;
                    break;
                }
            }

            if (allOnOneSide) {
                hullSet.insert(m_points[i]);
                hullSet.insert(m_points[j]);
            }
        }
    }

    m_hull.clear();
    for (const QPoint& p : hullSet) {
        m_hull.append(p);
    }

    if (m_hull.size() > 2) {
        QVector<QPoint> sortedHull;
        int l = 0;
        for (int i = 1; i < m_hull.size(); i++)
            if (m_hull[i].x() < m_hull[l].x())
                l = i;

        int p = l, q;
        int h = m_hull.size();
        do {
            sortedHull.append(m_hull[p]);
            q = (p + 1) % h;

            for (int i = 0; i < h; i++) {
                if (orientation(m_hull[p], m_hull[i], m_hull[q]) == 2)
                    q = i;
            }
            p = q;
        } while (p != l);
        m_hull = sortedHull;
    }
}

void DrawingCanvas::runGrahamScan()
{
    m_hull.clear();
    m_iterations = 0;
    m_algorithmName = "Fast Hull (Graham Scan O(n log n))";
    QVector<QPoint> localPoints = m_points;

    int n = localPoints.size();
    if (n < 3) {
        m_hull = localPoints;
        return;
    }

    int minY = localPoints[0].y(), minIndex = 0;
    for (int i = 1; i < n; i++) {
        m_iterations++;
        int y = localPoints[i].y();
        if ((y < minY) || (minY == y && localPoints[i].x() < localPoints[minIndex].x())) {
            minY = localPoints[i].y();
            minIndex = i;
        }
    }

    p0 = localPoints[minIndex];
    std::swap(localPoints[0], localPoints[minIndex]);

    std::sort(localPoints.begin() + 1, localPoints.end(), comparePoints);

    QVector<QPoint> S;
    S.append(localPoints[0]);
    S.append(localPoints[1]);

    if (n < 3) return;
    S.append(localPoints[2]);

    for (int i = 3; i < n; i++) {
        m_iterations++;
        while (S.size() > 1 && orientation(S[S.size()-2], S.last(), localPoints[i]) != 2)
        {
            S.pop_back();
            m_iterations++;
        }
        S.append(localPoints[i]);
    }

    m_hull = S;
}

void DrawingCanvas::runJarvisMarch()
{
    m_hull.clear();
    m_iterations = 0;
    m_algorithmName = "Convex Hull (Jarvis's March O(nh))";
    int n = m_points.size();

    if (n < 3) {
        m_hull = m_points;
        return;
    }

    int l = 0;
    for (int i = 1; i < n; i++) {
        if (m_points[i].x() < m_points[l].x())
            l = i;
        m_iterations++;
    }

    int p = l, q;
    do
    {
        m_hull.append(m_points[p]);
        q = (p + 1) % n;

        for (int i = 0; i < n; i++)
        {
            m_iterations++;
            if (orientation(m_points[p], m_points[i], m_points[q]) == 2)
                q = i;
        }
        p = q;

    } while (p != l);
}
