#include "ui/dialogs/ParagraphDialog.h"
#include "core/utils/Constants.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QTextEdit>
#include <QFrame>
#include <QDebug>

namespace QtWordEditor {

class ParagraphDialog::Private
{
public:
    QComboBox *alignmentCombo = nullptr;
    QDoubleSpinBox *firstLineIndentSpin = nullptr;
    QDoubleSpinBox *leftIndentSpin = nullptr;
    QDoubleSpinBox *rightIndentSpin = nullptr;
    QDoubleSpinBox *spaceBeforeSpin = nullptr;
    QDoubleSpinBox *spaceAfterSpin = nullptr;
    QDoubleSpinBox *lineHeightSpin = nullptr;
    QTextEdit *previewTextEdit = nullptr;
    
    ParagraphStyle currentStyle;
    ParagraphStyle initialStyle;
    
    void updatePreview();
};

ParagraphDialog::ParagraphDialog(QWidget *parent)
    : QDialog(parent)
    , d(new Private)
{
    setWindowTitle(tr("段落"));
    setMinimumSize(500, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Alignment group
    QGroupBox *alignmentGroup = new QGroupBox(tr("对齐方式"), this);
    QHBoxLayout *alignLayout = new QHBoxLayout(alignmentGroup);
    d->alignmentCombo = new QComboBox(alignmentGroup);
    d->alignmentCombo->addItem(tr("左对齐"), static_cast<int>(QtWordEditor::ParagraphAlignment::AlignLeft));
    d->alignmentCombo->addItem(tr("居中对齐"), static_cast<int>(QtWordEditor::ParagraphAlignment::AlignCenter));
    d->alignmentCombo->addItem(tr("右对齐"), static_cast<int>(QtWordEditor::ParagraphAlignment::AlignRight));
    d->alignmentCombo->addItem(tr("两端对齐"), static_cast<int>(QtWordEditor::ParagraphAlignment::AlignJustify));
    d->alignmentCombo->addItem(tr("分散对齐"), static_cast<int>(QtWordEditor::ParagraphAlignment::AlignDistributed));
    alignLayout->addWidget(d->alignmentCombo);
    alignmentGroup->setLayout(alignLayout);
    mainLayout->addWidget(alignmentGroup);

    // Indentation group
    QGroupBox *indentGroup = new QGroupBox(tr("缩进"), this);
    QGridLayout *indentLayout = new QGridLayout(indentGroup);
    
    indentLayout->addWidget(new QLabel(tr("首行缩进:"), indentGroup), 0, 0);
    d->firstLineIndentSpin = new QDoubleSpinBox(indentGroup);
    d->firstLineIndentSpin->setRange(-200, 200);
    d->firstLineIndentSpin->setValue(Constants::DEFAULT_FIRST_LINE_INDENT);
    d->firstLineIndentSpin->setSingleStep(6.0);
    d->firstLineIndentSpin->setSuffix(" pt");
    indentLayout->addWidget(d->firstLineIndentSpin, 0, 1);
    
    indentLayout->addWidget(new QLabel(tr("左缩进:"), indentGroup), 1, 0);
    d->leftIndentSpin = new QDoubleSpinBox(indentGroup);
    d->leftIndentSpin->setRange(0, 200);
    d->leftIndentSpin->setValue(Constants::DEFAULT_LEFT_INDENT);
    d->leftIndentSpin->setSingleStep(6.0);
    d->leftIndentSpin->setSuffix(" pt");
    indentLayout->addWidget(d->leftIndentSpin, 1, 1);
    
    indentLayout->addWidget(new QLabel(tr("右缩进:"), indentGroup), 2, 0);
    d->rightIndentSpin = new QDoubleSpinBox(indentGroup);
    d->rightIndentSpin->setRange(0, 200);
    d->rightIndentSpin->setValue(Constants::DEFAULT_RIGHT_INDENT);
    d->rightIndentSpin->setSingleStep(6.0);
    d->rightIndentSpin->setSuffix(" pt");
    indentLayout->addWidget(d->rightIndentSpin, 2, 1);
    
    indentGroup->setLayout(indentLayout);
    mainLayout->addWidget(indentGroup);

    // Spacing group
    QGroupBox *spacingGroup = new QGroupBox(tr("间距"), this);
    QGridLayout *spacingLayout = new QGridLayout(spacingGroup);
    
    spacingLayout->addWidget(new QLabel(tr("段前:"), spacingGroup), 0, 0);
    d->spaceBeforeSpin = new QDoubleSpinBox(spacingGroup);
    d->spaceBeforeSpin->setRange(0, 200);
    d->spaceBeforeSpin->setValue(Constants::DEFAULT_SPACE_BEFORE);
    d->spaceBeforeSpin->setSingleStep(6.0);
    d->spaceBeforeSpin->setSuffix(" pt");
    spacingLayout->addWidget(d->spaceBeforeSpin, 0, 1);
    
    spacingLayout->addWidget(new QLabel(tr("段后:"), spacingGroup), 1, 0);
    d->spaceAfterSpin = new QDoubleSpinBox(spacingGroup);
    d->spaceAfterSpin->setRange(0, 200);
    d->spaceAfterSpin->setValue(Constants::DEFAULT_SPACE_AFTER);
    d->spaceAfterSpin->setSingleStep(6.0);
    d->spaceAfterSpin->setSuffix(" pt");
    spacingLayout->addWidget(d->spaceAfterSpin, 1, 1);
    
    spacingLayout->addWidget(new QLabel(tr("行高:"), spacingGroup), 2, 0);
    d->lineHeightSpin = new QDoubleSpinBox(spacingGroup);
    d->lineHeightSpin->setRange(50, 300);
    d->lineHeightSpin->setValue(Constants::DEFAULT_LINE_HEIGHT_PERCENT);
    d->lineHeightSpin->setSingleStep(10);
    d->lineHeightSpin->setSuffix(" %");
    spacingLayout->addWidget(d->lineHeightSpin, 2, 1);
    
    spacingGroup->setLayout(spacingLayout);
    mainLayout->addWidget(spacingGroup);

    // Preview group
    QGroupBox *previewGroup = new QGroupBox(tr("预览"), this);
    QVBoxLayout *previewLayout = new QVBoxLayout(previewGroup);
    d->previewTextEdit = new QTextEdit(previewGroup);
    d->previewTextEdit->setReadOnly(true);
    d->previewTextEdit->setMinimumHeight(150);
    d->previewTextEdit->setPlainText(tr("这是一段示例文本，用于预览段落格式设置效果。\n\n"
                                       "您可以通过调整上面的参数来查看不同的段落格式效果。\n"
                                       "所有修改都会在预览区域实时显示。"));
    previewLayout->addWidget(d->previewTextEdit);
    previewGroup->setLayout(previewLayout);
    mainLayout->addWidget(previewGroup);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    QPushButton *applyButton = new QPushButton(tr("应用"), this);
    QPushButton *okButton = new QPushButton(tr("确定"), this);
    QPushButton *cancelButton = new QPushButton(tr("取消"), this);
    
    applyButton->setDefault(false);
    okButton->setDefault(true);
    
    buttonLayout->addWidget(applyButton);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    // Connect signals
    connect(d->alignmentCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() { updateCurrentStyle(); d->updatePreview(); });
    connect(d->firstLineIndentSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() { updateCurrentStyle(); d->updatePreview(); });
    connect(d->leftIndentSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() { updateCurrentStyle(); d->updatePreview(); });
    connect(d->rightIndentSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() { updateCurrentStyle(); d->updatePreview(); });
    connect(d->spaceBeforeSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() { updateCurrentStyle(); d->updatePreview(); });
    connect(d->spaceAfterSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() { updateCurrentStyle(); d->updatePreview(); });
    connect(d->lineHeightSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this]() { updateCurrentStyle(); d->updatePreview(); });
    
    connect(applyButton, &QPushButton::clicked, this, [this]() { emit applied(); });
    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

ParagraphDialog::~ParagraphDialog()
{
}

void ParagraphDialog::setInitialStyle(const ParagraphStyle &style)
{
    d->initialStyle = style;
    d->currentStyle = style;
    
    // 设置控件值
    int alignIndex = d->alignmentCombo->findData(static_cast<int>(style.alignment()));
    if (alignIndex >= 0) {
        d->alignmentCombo->setCurrentIndex(alignIndex);
    }
    
    d->firstLineIndentSpin->setValue(style.firstLineIndent());
    d->leftIndentSpin->setValue(style.leftIndent());
    d->rightIndentSpin->setValue(style.rightIndent());
    d->spaceBeforeSpin->setValue(style.spaceBefore());
    d->spaceAfterSpin->setValue(style.spaceAfter());
    d->lineHeightSpin->setValue(style.lineHeight());
    
    d->updatePreview();
}

ParagraphStyle ParagraphDialog::getStyle(const ParagraphStyle &initial, QWidget *parent)
{
    ParagraphDialog dialog(parent);
    dialog.setInitialStyle(initial);
    
    if (dialog.exec() == QDialog::Accepted) {
        return dialog.selectedStyle();
    }
    
    return initial;
}

ParagraphStyle ParagraphDialog::selectedStyle() const
{
    return d->currentStyle;
}

void ParagraphDialog::updateCurrentStyle()
{
    d->currentStyle.setAlignment(static_cast<QtWordEditor::ParagraphAlignment>(
        d->alignmentCombo->currentData().toInt()));
    d->currentStyle.setFirstLineIndent(d->firstLineIndentSpin->value());
    d->currentStyle.setLeftIndent(d->leftIndentSpin->value());
    d->currentStyle.setRightIndent(d->rightIndentSpin->value());
    d->currentStyle.setSpaceBefore(d->spaceBeforeSpin->value());
    d->currentStyle.setSpaceAfter(d->spaceAfterSpin->value());
    d->currentStyle.setLineHeight(d->lineHeightSpin->value());
}

void ParagraphDialog::Private::updatePreview()
{
    // 构建HTML预览
    QString html;
    
    // 应用对齐方式
    QString textAlign;
    switch (currentStyle.alignment()) {
        case ParagraphAlignment::AlignLeft:
            textAlign = "left";
            break;
        case ParagraphAlignment::AlignCenter:
            textAlign = "center";
            break;
        case ParagraphAlignment::AlignRight:
            textAlign = "right";
            break;
        case ParagraphAlignment::AlignJustify:
        case ParagraphAlignment::AlignDistributed:
            textAlign = "justify";
            break;
    }
    
    QStringList styles;
    styles << QString("text-align: %1").arg(textAlign);
    styles << QString("text-indent: %1pt").arg(currentStyle.firstLineIndent());
    styles << QString("padding-left: %1pt").arg(currentStyle.leftIndent());
    styles << QString("padding-right: %1pt").arg(currentStyle.rightIndent());
    styles << QString("margin-top: %1pt").arg(currentStyle.spaceBefore());
    styles << QString("margin-bottom: %1pt").arg(currentStyle.spaceAfter());
    styles << QString("line-height: %1%").arg(currentStyle.lineHeight());
    
    html = QString("<div style=\"%1\">"
                   "<p>这是一段示例文本，用于预览段落格式设置效果。</p>"
                   "<p>您可以通过调整上面的参数来查看不同的段落格式效果。</p>"
                   "<p>所有修改都会在预览区域实时显示。</p>"
                   "</div>").arg(styles.join("; "));
    
    previewTextEdit->setHtml(html);
}

} // namespace QtWordEditor