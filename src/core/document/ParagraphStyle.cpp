#include "include/core/document/ParagraphStyle.h"
#include <QDebug>

namespace QtWordEditor {

class ParagraphStyleData : public QSharedData
{
public:
    ParagraphStyleData()
        : m_alignment(ParagraphAlignment::AlignLeft)
        , m_firstLineIndent(0.0)
        , m_leftIndent(0.0)
        , m_rightIndent(0.0)
        , m_spaceBefore(0.0)
        , m_spaceAfter(0.0)
        , m_lineHeight(100)
    {
    }

    ParagraphStyleData(const ParagraphStyleData &other)
        : QSharedData(other)
        , m_alignment(other.m_alignment)
        , m_firstLineIndent(other.m_firstLineIndent)
        , m_leftIndent(other.m_leftIndent)
        , m_rightIndent(other.m_rightIndent)
        , m_spaceBefore(other.m_spaceBefore)
        , m_spaceAfter(other.m_spaceAfter)
        , m_lineHeight(other.m_lineHeight)
    {
    }

    ~ParagraphStyleData() = default;

    ParagraphAlignment m_alignment;
    qreal m_firstLineIndent;
    qreal m_leftIndent;
    qreal m_rightIndent;
    qreal m_spaceBefore;
    qreal m_spaceAfter;
    int m_lineHeight;
};

ParagraphStyle::ParagraphStyle()
    : d(new ParagraphStyleData)
{
}

ParagraphStyle::ParagraphStyle(const ParagraphStyle &other)
    : d(other.d)
{
}

ParagraphStyle &ParagraphStyle::operator=(const ParagraphStyle &other)
{
    if (this != &other)
        d = other.d;
    return *this;
}

ParagraphStyle::~ParagraphStyle()
{
}

ParagraphAlignment ParagraphStyle::alignment() const
{
    return d->m_alignment;
}

void ParagraphStyle::setAlignment(ParagraphAlignment align)
{
    d->m_alignment = align;
}

qreal ParagraphStyle::firstLineIndent() const
{
    return d->m_firstLineIndent;
}

void ParagraphStyle::setFirstLineIndent(qreal indent)
{
    d->m_firstLineIndent = indent;
}

qreal ParagraphStyle::leftIndent() const
{
    return d->m_leftIndent;
}

void ParagraphStyle::setLeftIndent(qreal indent)
{
    d->m_leftIndent = indent;
}

qreal ParagraphStyle::rightIndent() const
{
    return d->m_rightIndent;
}

void ParagraphStyle::setRightIndent(qreal indent)
{
    d->m_rightIndent = indent;
}

qreal ParagraphStyle::spaceBefore() const
{
    return d->m_spaceBefore;
}

void ParagraphStyle::setSpaceBefore(qreal space)
{
    d->m_spaceBefore = space;
}

qreal ParagraphStyle::spaceAfter() const
{
    return d->m_spaceAfter;
}

void ParagraphStyle::setSpaceAfter(qreal space)
{
    d->m_spaceAfter = space;
}

int ParagraphStyle::lineHeight() const
{
    return d->m_lineHeight;
}

void ParagraphStyle::setLineHeight(int percent)
{
    d->m_lineHeight = percent;
}

bool ParagraphStyle::operator==(const ParagraphStyle &other) const
{
    return d->m_alignment == other.d->m_alignment &&
           qFuzzyCompare(d->m_firstLineIndent, other.d->m_firstLineIndent) &&
           qFuzzyCompare(d->m_leftIndent, other.d->m_leftIndent) &&
           qFuzzyCompare(d->m_rightIndent, other.d->m_rightIndent) &&
           qFuzzyCompare(d->m_spaceBefore, other.d->m_spaceBefore) &&
           qFuzzyCompare(d->m_spaceAfter, other.d->m_spaceAfter) &&
           d->m_lineHeight == other.d->m_lineHeight;
}

bool ParagraphStyle::operator!=(const ParagraphStyle &other) const
{
    return !(*this == other);
}

void ParagraphStyle::reset()
{
    d = new ParagraphStyleData;
}

} // namespace QtWordEditor