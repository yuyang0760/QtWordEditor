#include "core/document/ParagraphStyle.h"
#include "core/utils/Constants.h"
#include <QDebug>

namespace QtWordEditor {

class ParagraphStyleData : public QSharedData
{
public:
    ParagraphStyleData()
        : m_alignment(QtWordEditor::Constants::DEFAULT_ALIGNMENT)
        , m_firstLineIndent(QtWordEditor::Constants::DEFAULT_FIRST_LINE_INDENT)
        , m_leftIndent(QtWordEditor::Constants::DEFAULT_LEFT_INDENT)
        , m_rightIndent(QtWordEditor::Constants::DEFAULT_RIGHT_INDENT)
        , m_spaceBefore(QtWordEditor::Constants::DEFAULT_SPACE_BEFORE)
        , m_spaceAfter(QtWordEditor::Constants::DEFAULT_SPACE_AFTER)
        , m_lineHeight(QtWordEditor::Constants::DEFAULT_LINE_HEIGHT_PERCENT)
        , m_propertySetFlags()
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
        , m_propertySetFlags(other.m_propertySetFlags)
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
    ParagraphStylePropertyFlags m_propertySetFlags;  ///< 属性设置标记
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
    if (d->m_alignment != align) {
        d->m_alignment = align;
        d->m_propertySetFlags |= ParagraphStyleProperty::Alignment;
    }
}

qreal ParagraphStyle::firstLineIndent() const
{
    return d->m_firstLineIndent;
}

void ParagraphStyle::setFirstLineIndent(qreal indent)
{
    if (!qFuzzyCompare(d->m_firstLineIndent, indent)) {
        d->m_firstLineIndent = indent;
        d->m_propertySetFlags |= ParagraphStyleProperty::FirstLineIndent;
    }
}

qreal ParagraphStyle::leftIndent() const
{
    return d->m_leftIndent;
}

void ParagraphStyle::setLeftIndent(qreal indent)
{
    if (!qFuzzyCompare(d->m_leftIndent, indent)) {
        d->m_leftIndent = indent;
        d->m_propertySetFlags |= ParagraphStyleProperty::LeftIndent;
    }
}

qreal ParagraphStyle::rightIndent() const
{
    return d->m_rightIndent;
}

void ParagraphStyle::setRightIndent(qreal indent)
{
    if (!qFuzzyCompare(d->m_rightIndent, indent)) {
        d->m_rightIndent = indent;
        d->m_propertySetFlags |= ParagraphStyleProperty::RightIndent;
    }
}

qreal ParagraphStyle::spaceBefore() const
{
    return d->m_spaceBefore;
}

void ParagraphStyle::setSpaceBefore(qreal space)
{
    if (!qFuzzyCompare(d->m_spaceBefore, space)) {
        d->m_spaceBefore = space;
        d->m_propertySetFlags |= ParagraphStyleProperty::SpaceBefore;
    }
}

qreal ParagraphStyle::spaceAfter() const
{
    return d->m_spaceAfter;
}

void ParagraphStyle::setSpaceAfter(qreal space)
{
    if (!qFuzzyCompare(d->m_spaceAfter, space)) {
        d->m_spaceAfter = space;
        d->m_propertySetFlags |= ParagraphStyleProperty::SpaceAfter;
    }
}

int ParagraphStyle::lineHeight() const
{
    return d->m_lineHeight;
}

void ParagraphStyle::setLineHeight(int percent)
{
    if (d->m_lineHeight != percent) {
        d->m_lineHeight = percent;
        d->m_propertySetFlags |= ParagraphStyleProperty::LineHeight;
    }
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

bool ParagraphStyle::isPropertySet(ParagraphStyleProperty property) const
{
    return d->m_propertySetFlags.testFlag(property);
}

void ParagraphStyle::clearProperty(ParagraphStyleProperty property)
{
    // 清除属性标记
    d->m_propertySetFlags &= ~ParagraphStylePropertyFlags(property);
    
    // 将属性恢复为默认值
    switch (property) {
        case ParagraphStyleProperty::Alignment:
            d->m_alignment = QtWordEditor::Constants::DEFAULT_ALIGNMENT;
            break;
        case ParagraphStyleProperty::FirstLineIndent:
            d->m_firstLineIndent = QtWordEditor::Constants::DEFAULT_FIRST_LINE_INDENT;
            break;
        case ParagraphStyleProperty::LeftIndent:
            d->m_leftIndent = QtWordEditor::Constants::DEFAULT_LEFT_INDENT;
            break;
        case ParagraphStyleProperty::RightIndent:
            d->m_rightIndent = QtWordEditor::Constants::DEFAULT_RIGHT_INDENT;
            break;
        case ParagraphStyleProperty::SpaceBefore:
            d->m_spaceBefore = QtWordEditor::Constants::DEFAULT_SPACE_BEFORE;
            break;
        case ParagraphStyleProperty::SpaceAfter:
            d->m_spaceAfter = QtWordEditor::Constants::DEFAULT_SPACE_AFTER;
            break;
        case ParagraphStyleProperty::LineHeight:
            d->m_lineHeight = QtWordEditor::Constants::DEFAULT_LINE_HEIGHT_PERCENT;
            break;
    }
}

void ParagraphStyle::clearAllProperties()
{
    d = new ParagraphStyleData;
}

ParagraphStyle ParagraphStyle::mergeWith(const ParagraphStyle &other) const
{
    ParagraphStyle result = *this;
    
    // 只合并另一个样式中已显式设置的属性
    if (other.isPropertySet(ParagraphStyleProperty::Alignment)) {
        result.setAlignment(other.alignment());
    }
    if (other.isPropertySet(ParagraphStyleProperty::FirstLineIndent)) {
        result.setFirstLineIndent(other.firstLineIndent());
    }
    if (other.isPropertySet(ParagraphStyleProperty::LeftIndent)) {
        result.setLeftIndent(other.leftIndent());
    }
    if (other.isPropertySet(ParagraphStyleProperty::RightIndent)) {
        result.setRightIndent(other.rightIndent());
    }
    if (other.isPropertySet(ParagraphStyleProperty::SpaceBefore)) {
        result.setSpaceBefore(other.spaceBefore());
    }
    if (other.isPropertySet(ParagraphStyleProperty::SpaceAfter)) {
        result.setSpaceAfter(other.spaceAfter());
    }
    if (other.isPropertySet(ParagraphStyleProperty::LineHeight)) {
        result.setLineHeight(other.lineHeight());
    }
    
    return result;
}

} // namespace QtWordEditor