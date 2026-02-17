#ifndef RULER_H
#define RULER_H

#include <QWidget>
#include "core/Global.h"

namespace QtWordEditor {

class RulerPrivate;

/**
 * @brief The Ruler class displays a horizontal ruler showing page margins,
 * paragraph indents, and tab stops.
 */
class Ruler : public QWidget
{
    Q_OBJECT
public:
    explicit Ruler(QWidget *parent = nullptr);
    ~Ruler() override;

    // Set the view for coordinate transformations.
    void setView(QWidget *view);

    // Set page margins (in millimeters).
    void setMargins(qreal left, qreal right);

    // Set paragraph indents (in millimeters).
    void setIndents(qreal firstLine, qreal left);

    // Get current settings.
    qreal leftMargin() const;
    qreal rightMargin() const;
    qreal firstLineIndent() const;
    qreal leftIndent() const;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QScopedPointer<RulerPrivate> d;
};

} // namespace QtWordEditor

#endif // RULER_H