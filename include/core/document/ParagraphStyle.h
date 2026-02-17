#ifndef PARAGRAPHSTYLE_H
#define PARAGRAPHSTYLE_H

#include <QSharedData>
#include <QSharedDataPointer>
#include "core/Global.h"

namespace QtWordEditor {

class ParagraphStyleData;

/**
 * @brief The ParagraphStyle class encapsulates paragraph-level formatting attributes.
 */
class ParagraphStyle
{
public:
    ParagraphStyle();
    ParagraphStyle(const ParagraphStyle &other);
    ParagraphStyle &operator=(const ParagraphStyle &other);
    ~ParagraphStyle();

    // Alignment
    ParagraphAlignment alignment() const;
    void setAlignment(ParagraphAlignment align);

    // Indentation (in millimeters or points)
    qreal firstLineIndent() const;
    void setFirstLineIndent(qreal indent);

    qreal leftIndent() const;
    void setLeftIndent(qreal indent);

    qreal rightIndent() const;
    void setRightIndent(qreal indent);

    // Spacing (in millimeters or points)
    qreal spaceBefore() const;
    void setSpaceBefore(qreal space);

    qreal spaceAfter() const;
    void setSpaceAfter(qreal space);

    // Line height (percentage, e.g., 150 for 1.5 lines)
    int lineHeight() const;
    void setLineHeight(int percent);

    // Comparison
    bool operator==(const ParagraphStyle &other) const;
    bool operator!=(const ParagraphStyle &other) const;

    // Reset to default
    void reset();

private:
    QSharedDataPointer<ParagraphStyleData> d;
};

} // namespace QtWordEditor

#endif // PARAGRAPHSTYLE_H