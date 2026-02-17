#include "ui/widgets/Ruler.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

namespace QtWordEditor {

class RulerPrivate
{
public:
    QWidget *view = nullptr;
    qreal leftMargin = 20.0;   // millimeters
    qreal rightMargin = 20.0;
    qreal firstLineIndent = 0.0;
    qreal leftIndent = 0.0;
    QList<qreal> tabStops;     // in millimeters

    // For dragging
    bool draggingLeftMargin = false;
    bool draggingRightMargin = false;
    bool draggingFirstLine = false;
    bool draggingLeftIndent = false;
    QPoint dragStartPos;
};

Ruler::Ruler(QWidget *parent)
    : QWidget(parent)
    , d(new RulerPrivate)
{
    setFixedHeight(30);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

Ruler::~Ruler()
{
}

void Ruler::setView(QWidget *view)
{
    d->view = view;
}

void Ruler::setMargins(qreal left, qreal right)
{
    if (qFuzzyCompare(d->leftMargin, left) && qFuzzyCompare(d->rightMargin, right))
        return;
    d->leftMargin = left;
    d->rightMargin = right;
    update();
}

void Ruler::setIndents(qreal firstLine, qreal left)
{
    if (qFuzzyCompare(d->firstLineIndent, firstLine) && qFuzzyCompare(d->leftIndent, left))
        return;
    d->firstLineIndent = firstLine;
    d->leftIndent = left;
    update();
}

qreal Ruler::leftMargin() const
{
    return d->leftMargin;
}

qreal Ruler::rightMargin() const
{
    return d->rightMargin;
}

qreal Ruler::firstLineIndent() const
{
    return d->firstLineIndent;
}

qreal Ruler::leftIndent() const
{
    return d->leftIndent;
}

void Ruler::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);

    // Draw ruler background
    painter.setPen(Qt::lightGray);
    for (int i = 0; i < width(); i += 10) {
        painter.drawLine(i, height() - 5, i, height());
    }
    for (int i = 0; i < width(); i += 50) {
        painter.drawLine(i, height() - 10, i, height());
        painter.drawText(QRect(i - 10, 0, 20, height() - 10),
                         Qt::AlignCenter, QString::number(i / 10));
    }

    // Draw margins (simplified: assume 1 mm = 1 pixel for demo)
    qreal leftPx = d->leftMargin;
    qreal rightPx = width() - d->rightMargin;
    painter.setPen(Qt::blue);
    painter.drawLine(leftPx, 0, leftPx, height());
    painter.drawLine(rightPx, 0, rightPx, height());

    // Draw indents
    painter.setPen(Qt::green);
    qreal firstLinePx = leftPx + d->firstLineIndent;
    qreal leftIndentPx = leftPx + d->leftIndent;
    painter.drawLine(firstLinePx, 0, firstLinePx, height() - 10);
    painter.drawLine(leftIndentPx, 0, leftIndentPx, height() - 10);

    // Draw tab stops
    painter.setPen(Qt::red);
    foreach (qreal tab, d->tabStops) {
        qreal tabPx = leftPx + tab;
        painter.drawLine(tabPx, 0, tabPx, height() - 15);
    }
}

void Ruler::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        d->dragStartPos = event->pos();
        // Detect what is being dragged (simplified)
        qreal leftPx = d->leftMargin;
        qreal rightPx = width() - d->rightMargin;
        qreal firstLinePx = leftPx + d->firstLineIndent;
        qreal leftIndentPx = leftPx + d->leftIndent;

        if (qAbs(event->x() - leftPx) < 5) {
            d->draggingLeftMargin = true;
        } else if (qAbs(event->x() - rightPx) < 5) {
            d->draggingRightMargin = true;
        } else if (qAbs(event->x() - firstLinePx) < 5) {
            d->draggingFirstLine = true;
        } else if (qAbs(event->x() - leftIndentPx) < 5) {
            d->draggingLeftIndent = true;
        }
    }
    QWidget::mousePressEvent(event);
}

void Ruler::mouseMoveEvent(QMouseEvent *event)
{
    if (d->draggingLeftMargin) {
        d->leftMargin = qMax(0.0, static_cast<double>(event->position().x()));
        update();
    } else if (d->draggingRightMargin) {
        d->rightMargin = qMax(0.0, static_cast<double>(width() - event->position().x()));
        update();
    } else if (d->draggingFirstLine) {
        d->firstLineIndent = qMax(0.0, static_cast<double>(event->position().x() - d->leftMargin));
        update();
    } else if (d->draggingLeftIndent) {
        d->leftIndent = qMax(0.0, event->x() - d->leftMargin);
        update();
    }
    QWidget::mouseMoveEvent(event);
}

void Ruler::mouseReleaseEvent(QMouseEvent *event)
{
    d->draggingLeftMargin = false;
    d->draggingRightMargin = false;
    d->draggingFirstLine = false;
    d->draggingLeftIndent = false;
    QWidget::mouseReleaseEvent(event);
}

} // namespace QtWordEditor