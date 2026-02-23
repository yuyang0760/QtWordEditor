/**
 * @file PageSetupDialog.cpp
 * @brief 页面设置对话框实现文件
 * @details 提供页面尺寸、方向、边距等设置功能，并支持实时预览
 */

#include "ui/dialogs/PageSetupDialog.h"
#include "core/utils/Constants.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QFrame>
#include <QTextEdit>
#include <tuple>

namespace QtWordEditor {

/**
 * @brief PageSetupDialog 私有实现类
 * @details 使用 Pimpl 模式封装内部实现细节
 */
class PageSetupDialog::Private
{
public:
    // 页面尺寸相关控件
    QComboBox *pageSizeCombo = nullptr;        ///< 页面尺寸预设下拉框
    QDoubleSpinBox *pageWidthSpin = nullptr;   ///< 页面宽度输入框
    QDoubleSpinBox *pageHeightSpin = nullptr;  ///< 页面高度输入框
    
    // 页面方向相关控件
    QRadioButton *portraitRadio = nullptr;     ///< 纵向单选按钮
    QRadioButton *landscapeRadio = nullptr;    ///< 横向单选按钮
    
    // 边距相关控件
    QComboBox *marginPresetCombo = nullptr;    ///< 边距预设下拉框
    QDoubleSpinBox *marginLeftSpin = nullptr;  ///< 左边距输入框
    QDoubleSpinBox *marginRightSpin = nullptr; ///< 右边距输入框
    QDoubleSpinBox *marginTopSpin = nullptr;   ///< 上边距输入框
    QDoubleSpinBox *marginBottomSpin = nullptr;///< 下边距输入框
    
    // 预览相关控件
    QTextEdit *previewTextEdit = nullptr;      ///< 实时预览文本框
    
    // 按钮
    QPushButton *okButton = nullptr;           ///< 确定按钮
    QPushButton *cancelButton = nullptr;       ///< 取消按钮
    
    // 状态标志
    bool isSettingFromPreset = false;          ///< 是否正在从预设设置值（防止递归调用）
    
    // 初始设置
    PageSetup initialSetup;                     ///< 初始页面设置
    
    /**
     * @brief 根据页面尺寸预设获取尺寸
     * @param preset 页面尺寸预设
     * @return 尺寸对 (width, height)，单位：毫米
     */
    QPair<qreal, qreal> getPageSizeForPreset(PageSizePreset preset) const
    {
        switch (preset) {
            case PageSizePreset::A4:
                return {Constants::PAGE_SIZE_A4_WIDTH, Constants::PAGE_SIZE_A4_HEIGHT};
            case PageSizePreset::A3:
                return {Constants::PAGE_SIZE_A3_WIDTH, Constants::PAGE_SIZE_A3_HEIGHT};
            case PageSizePreset::A5:
                return {Constants::PAGE_SIZE_A5_WIDTH, Constants::PAGE_SIZE_A5_HEIGHT};
            case PageSizePreset::Letter:
                return {Constants::PAGE_SIZE_LETTER_WIDTH, Constants::PAGE_SIZE_LETTER_HEIGHT};
            case PageSizePreset::Legal:
                return {Constants::PAGE_SIZE_LEGAL_WIDTH, Constants::PAGE_SIZE_LEGAL_HEIGHT};
            case PageSizePreset::Tabloid:
                return {Constants::PAGE_SIZE_TABLOID_WIDTH, Constants::PAGE_SIZE_TABLOID_HEIGHT};
            case PageSizePreset::Custom:
            default:
                return {0.0, 0.0};
        }
    }
    
    /**
     * @brief 根据边距预设获取边距值
     * @param preset 边距预设
     * @return 边距四元组 (left, right, top, bottom)，单位：毫米
     */
    std::tuple<qreal, qreal, qreal, qreal> getMarginsForPreset(MarginPreset preset) const
    {
        switch (preset) {
            case MarginPreset::Normal:
                return {Constants::MARGIN_NORMAL, Constants::MARGIN_NORMAL, 
                        Constants::MARGIN_NORMAL, Constants::MARGIN_NORMAL};
            case MarginPreset::Narrow:
                return {Constants::MARGIN_NARROW, Constants::MARGIN_NARROW, 
                        Constants::MARGIN_NARROW, Constants::MARGIN_NARROW};
            case MarginPreset::Moderate:
                return {Constants::MARGIN_MODERATE, Constants::MARGIN_MODERATE, 
                        Constants::MARGIN_MODERATE, Constants::MARGIN_MODERATE};
            case MarginPreset::Wide:
                return {Constants::MARGIN_WIDE, Constants::MARGIN_WIDE, 
                        Constants::MARGIN_WIDE, Constants::MARGIN_WIDE};
            case MarginPreset::Custom:
            default:
                return {0.0, 0.0, 0.0, 0.0};
        }
    }
    
    /**
     * @brief 更新实时预览
     * @details 根据当前设置更新预览区域的显示内容
     */
    void updatePreview()
    {
        if (!previewTextEdit) {
            return;
        }
        
        qreal width = pageWidthSpin->value();
        qreal height = pageHeightSpin->value();
        qreal left = marginLeftSpin->value();
        qreal right = marginRightSpin->value();
        qreal top = marginTopSpin->value();
        qreal bottom = marginBottomSpin->value();
        bool portrait = portraitRadio->isChecked();
        
        // 构建HTML预览
        QString html;
        
        // 页面方向
        QString orientationText = portrait ? QObject::tr("纵向") : QObject::tr("横向");
        
        // 构建页面尺寸显示
        QString pageSizeText = QObject::tr("页面尺寸: %1 × %2 mm").arg(width).arg(height);
        
        // 构建边距显示
        QString marginsText = QObject::tr("边距: 左 %1mm, 右 %2mm, 上 %3mm, 下 %4mm")
                                   .arg(left).arg(right).arg(top).arg(bottom);
        
        // 构建可视化预览HTML
        html = QString(
            "<html>"
            "<body style='font-family: Arial, sans-serif; font-size: 10pt; margin: 0; padding: 10px;'>"
            "<h3 style='margin: 0 0 10px 0; color: #333;'>%1</h3>"
            "<p style='margin: 5px 0; color: #555;'>%2</p>"
            "<p style='margin: 5px 0; color: #555;'>%3</p>"
            "<hr style='margin: 15px 0; border: none; border-top: 1px solid #ddd;'>"
            "<div style='border: 2px solid #4a90e2; padding: 15px; background-color: #f8f9fa;'>"
            "<h4 style='margin: 0 0 10px 0; color: #4a90e2;'>📄 页面预览</h4>"
            "<div style='background-color: white; border: 1px solid #ddd; padding: 20px;'>"
            "<p style='margin: 0; color: #666; line-height: 1.6;'>"
            "这是页面的预览区域。<br>"
            "您可以通过调整上方的参数来查看不同的页面设置效果。<br>"
            "所有修改都会在此预览区域实时显示。"
            "</p>"
            "</div>"
            "</div>"
            "</body>"
            "</html>"
        ).arg(orientationText, pageSizeText, marginsText);
        
        previewTextEdit->setHtml(html);
    }
    
    /**
     * @brief 初始化页面尺寸预设下拉框
     */
    void initPageSizeCombo()
    {
        pageSizeCombo->clear();
        pageSizeCombo->addItem(QObject::tr("A4"), QVariant::fromValue(PageSizePreset::A4));
        pageSizeCombo->addItem(QObject::tr("A3"), QVariant::fromValue(PageSizePreset::A3));
        pageSizeCombo->addItem(QObject::tr("A5"), QVariant::fromValue(PageSizePreset::A5));
        pageSizeCombo->addItem(QObject::tr("Letter"), QVariant::fromValue(PageSizePreset::Letter));
        pageSizeCombo->addItem(QObject::tr("Legal"), QVariant::fromValue(PageSizePreset::Legal));
        pageSizeCombo->addItem(QObject::tr("Tabloid"), QVariant::fromValue(PageSizePreset::Tabloid));
        pageSizeCombo->addItem(QObject::tr("Custom"), QVariant::fromValue(PageSizePreset::Custom));
    }
    
    /**
     * @brief 初始化边距预设下拉框
     */
    void initMarginPresetCombo()
    {
        marginPresetCombo->clear();
        marginPresetCombo->addItem(QObject::tr("Normal"), QVariant::fromValue(MarginPreset::Normal));
        marginPresetCombo->addItem(QObject::tr("Narrow"), QVariant::fromValue(MarginPreset::Narrow));
        marginPresetCombo->addItem(QObject::tr("Moderate"), QVariant::fromValue(MarginPreset::Moderate));
        marginPresetCombo->addItem(QObject::tr("Wide"), QVariant::fromValue(MarginPreset::Wide));
        marginPresetCombo->addItem(QObject::tr("Custom"), QVariant::fromValue(MarginPreset::Custom));
    }
};

/**
 * @brief 构造函数
 * @param parent 父窗口
 */
PageSetupDialog::PageSetupDialog(QWidget *parent)
    : QDialog(parent)
    , d(new Private)
{
    // 设置对话框属性
    setWindowTitle(tr("Page Setup"));
    setMinimumSize(500, 650);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);
    
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // ==========================================
    // 页面尺寸设置组
    // ==========================================
    QGroupBox *sizeGroup = new QGroupBox(tr("Page Size"), this);
    QGridLayout *sizeLayout = new QGridLayout(sizeGroup);
    sizeLayout->setSpacing(10);
    sizeLayout->setContentsMargins(15, 15, 15, 15);
    
    // 页面尺寸预设
    sizeLayout->addWidget(new QLabel(tr("Preset:"), sizeGroup), 0, 0);
    d->pageSizeCombo = new QComboBox(sizeGroup);
    d->initPageSizeCombo();
    sizeLayout->addWidget(d->pageSizeCombo, 0, 1, 1, 2);
    
    // 页面宽度
    sizeLayout->addWidget(new QLabel(tr("Width:"), sizeGroup), 1, 0);
    d->pageWidthSpin = new QDoubleSpinBox(sizeGroup);
    d->pageWidthSpin->setRange(Constants::PAGE_SIZE_MIN, Constants::PAGE_SIZE_MAX);
    d->pageWidthSpin->setValue(Constants::PAGE_SIZE_A4_WIDTH);
    d->pageWidthSpin->setSuffix(" mm");
    d->pageWidthSpin->setDecimals(Constants::PAGE_SIZE_DECIMALS);
    d->pageWidthSpin->setSingleStep(Constants::PAGE_SIZE_STEP);
    sizeLayout->addWidget(d->pageWidthSpin, 1, 1);
    
    // 页面高度
    sizeLayout->addWidget(new QLabel(tr("Height:"), sizeGroup), 2, 0);
    d->pageHeightSpin = new QDoubleSpinBox(sizeGroup);
    d->pageHeightSpin->setRange(Constants::PAGE_SIZE_MIN, Constants::PAGE_SIZE_MAX);
    d->pageHeightSpin->setValue(Constants::PAGE_SIZE_A4_HEIGHT);
    d->pageHeightSpin->setSuffix(" mm");
    d->pageHeightSpin->setDecimals(Constants::PAGE_SIZE_DECIMALS);
    d->pageHeightSpin->setSingleStep(Constants::PAGE_SIZE_STEP);
    sizeLayout->addWidget(d->pageHeightSpin, 2, 1);
    
    sizeGroup->setLayout(sizeLayout);
    mainLayout->addWidget(sizeGroup);
    
    // ==========================================
    // 页面方向设置组
    // ==========================================
    QGroupBox *orientGroup = new QGroupBox(tr("Orientation"), this);
    QHBoxLayout *orientLayout = new QHBoxLayout(orientGroup);
    orientLayout->setSpacing(20);
    orientLayout->setContentsMargins(15, 15, 15, 15);
    QButtonGroup *orientGroupBtn = new QButtonGroup(this);
    
    d->portraitRadio = new QRadioButton(tr("Portrait"), orientGroup);
    d->landscapeRadio = new QRadioButton(tr("Landscape"), orientGroup);
    d->portraitRadio->setChecked(true);
    
    orientGroupBtn->addButton(d->portraitRadio);
    orientGroupBtn->addButton(d->landscapeRadio);
    
    orientLayout->addWidget(d->portraitRadio);
    orientLayout->addWidget(d->landscapeRadio);
    orientLayout->addStretch();
    orientGroup->setLayout(orientLayout);
    mainLayout->addWidget(orientGroup);
    
    // ==========================================
    // 边距设置组
    // ==========================================
    QGroupBox *marginGroup = new QGroupBox(tr("Margins"), this);
    QGridLayout *marginLayout = new QGridLayout(marginGroup);
    marginLayout->setSpacing(10);
    marginLayout->setContentsMargins(15, 15, 15, 15);
    
    // 边距预设
    marginLayout->addWidget(new QLabel(tr("Preset:"), marginGroup), 0, 0);
    d->marginPresetCombo = new QComboBox(marginGroup);
    d->initMarginPresetCombo();
    marginLayout->addWidget(d->marginPresetCombo, 0, 1, 1, 2);
    
    // 左边距
    marginLayout->addWidget(new QLabel(tr("Left:"), marginGroup), 1, 0);
    d->marginLeftSpin = new QDoubleSpinBox(marginGroup);
    d->marginLeftSpin->setRange(Constants::MARGIN_MIN, Constants::MARGIN_MAX);
    d->marginLeftSpin->setValue(Constants::MARGIN_NORMAL);
    d->marginLeftSpin->setSuffix(" mm");
    d->marginLeftSpin->setDecimals(Constants::MARGIN_DECIMALS);
    d->marginLeftSpin->setSingleStep(Constants::MARGIN_STEP);
    marginLayout->addWidget(d->marginLeftSpin, 1, 1);
    
    // 右边距
    marginLayout->addWidget(new QLabel(tr("Right:"), marginGroup), 2, 0);
    d->marginRightSpin = new QDoubleSpinBox(marginGroup);
    d->marginRightSpin->setRange(Constants::MARGIN_MIN, Constants::MARGIN_MAX);
    d->marginRightSpin->setValue(Constants::MARGIN_NORMAL);
    d->marginRightSpin->setSuffix(" mm");
    d->marginRightSpin->setDecimals(Constants::MARGIN_DECIMALS);
    d->marginRightSpin->setSingleStep(Constants::MARGIN_STEP);
    marginLayout->addWidget(d->marginRightSpin, 2, 1);
    
    // 上边距
    marginLayout->addWidget(new QLabel(tr("Top:"), marginGroup), 3, 0);
    d->marginTopSpin = new QDoubleSpinBox(marginGroup);
    d->marginTopSpin->setRange(Constants::MARGIN_MIN, Constants::MARGIN_MAX);
    d->marginTopSpin->setValue(Constants::MARGIN_NORMAL);
    d->marginTopSpin->setSuffix(" mm");
    d->marginTopSpin->setDecimals(Constants::MARGIN_DECIMALS);
    d->marginTopSpin->setSingleStep(Constants::MARGIN_STEP);
    marginLayout->addWidget(d->marginTopSpin, 3, 1);
    
    // 下边距
    marginLayout->addWidget(new QLabel(tr("Bottom:"), marginGroup), 4, 0);
    d->marginBottomSpin = new QDoubleSpinBox(marginGroup);
    d->marginBottomSpin->setRange(Constants::MARGIN_MIN, Constants::MARGIN_MAX);
    d->marginBottomSpin->setValue(Constants::MARGIN_NORMAL);
    d->marginBottomSpin->setSuffix(" mm");
    d->marginBottomSpin->setDecimals(Constants::MARGIN_DECIMALS);
    d->marginBottomSpin->setSingleStep(Constants::MARGIN_STEP);
    marginLayout->addWidget(d->marginBottomSpin, 4, 1);
    
    marginGroup->setLayout(marginLayout);
    mainLayout->addWidget(marginGroup);
    
    // ==========================================
    // 实时预览区域
    // ==========================================
    QGroupBox *previewGroup = new QGroupBox(tr("Preview"), this);
    QVBoxLayout *previewLayout = new QVBoxLayout(previewGroup);
    previewLayout->setContentsMargins(15, 15, 15, 15);
    
    d->previewTextEdit = new QTextEdit(previewGroup);
    d->previewTextEdit->setReadOnly(true);
    d->previewTextEdit->setMaximumHeight(200);
    d->previewTextEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    previewLayout->addWidget(d->previewTextEdit);
    
    previewGroup->setLayout(previewLayout);
    mainLayout->addWidget(previewGroup);
    
    // ==========================================
    // 按钮区域
    // ==========================================
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    d->okButton = new QPushButton(tr("OK"), this);
    d->cancelButton = new QPushButton(tr("Cancel"), this);
    d->okButton->setDefault(true);
    d->okButton->setMinimumWidth(80);
    d->cancelButton->setMinimumWidth(80);
    buttonLayout->addWidget(d->okButton);
    buttonLayout->addWidget(d->cancelButton);
    mainLayout->addLayout(buttonLayout);
    
    // ==========================================
    // 连接信号和槽
    // ==========================================
    connectSignals();
    
    // 初始化预览
    d->updatePreview();
}

/**
 * @brief 析构函数
 */
PageSetupDialog::~PageSetupDialog()
{
}

/**
 * @brief 连接信号和槽
 */
void PageSetupDialog::connectSignals()
{
    // 按钮连接
    connect(d->okButton, &QPushButton::clicked, this, [this]() {
        accept();
    });
    connect(d->cancelButton, &QPushButton::clicked, this, [this]() {
        reject();
    });
    
    // 页面尺寸预设变化
    connect(d->pageSizeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PageSetupDialog::onPageSizePresetChanged);
    
    // 页面方向变化
    connect(d->portraitRadio, &QRadioButton::toggled,
            this, &PageSetupDialog::onOrientationChanged);
    connect(d->landscapeRadio, &QRadioButton::toggled,
            this, &PageSetupDialog::onOrientationChanged);
    
    // 边距预设变化
    connect(d->marginPresetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PageSetupDialog::onMarginPresetChanged);
    
    // 自定义尺寸变化
    connect(d->pageWidthSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PageSetupDialog::onCustomSizeChanged);
    connect(d->pageHeightSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PageSetupDialog::onCustomSizeChanged);
    
    // 自定义边距变化
    connect(d->marginLeftSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PageSetupDialog::onCustomMarginChanged);
    connect(d->marginRightSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PageSetupDialog::onCustomMarginChanged);
    connect(d->marginTopSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PageSetupDialog::onCustomMarginChanged);
    connect(d->marginBottomSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &PageSetupDialog::onCustomMarginChanged);
}

/**
 * @brief 处理页面尺寸预设变化
 * @param index 新的预设索引
 */
void PageSetupDialog::onPageSizePresetChanged(int index)
{
    if (d->isSettingFromPreset) {
        return;
    }
    
    PageSizePreset preset = d->pageSizeCombo->itemData(index).value<PageSizePreset>();
    d->isSettingFromPreset = true;
    
    // 获取预设尺寸
    QPair<qreal, qreal> size = d->getPageSizeForPreset(preset);
    
    if (preset != PageSizePreset::Custom) {
        qreal width = size.first;
        qreal height = size.second;
        
        // 如果是横向，交换宽高
        if (!d->portraitRadio->isChecked()) {
            qSwap(width, height);
        }
        
        d->pageWidthSpin->setValue(width);
        d->pageHeightSpin->setValue(height);
    }
    
    d->isSettingFromPreset = false;
    d->updatePreview();
}

/**
 * @brief 处理页面方向变化
 */
void PageSetupDialog::onOrientationChanged()
{
    if (d->isSettingFromPreset) {
        return;
    }
    
    d->isSettingFromPreset = true;
    
    bool isPortrait = d->portraitRadio->isChecked();
    
    // 交换宽高
    qreal width = d->pageWidthSpin->value();
    qreal height = d->pageHeightSpin->value();
    d->pageWidthSpin->setValue(height);
    d->pageHeightSpin->setValue(width);
    
    d->isSettingFromPreset = false;
    d->updatePreview();
}

/**
 * @brief 处理边距预设变化
 * @param index 新的预设索引
 */
void PageSetupDialog::onMarginPresetChanged(int index)
{
    if (d->isSettingFromPreset) {
        return;
    }
    
    MarginPreset preset = d->marginPresetCombo->itemData(index).value<MarginPreset>();
    d->isSettingFromPreset = true;
    
    // 获取预设边距
    auto margins = d->getMarginsForPreset(preset);
    
    if (preset != MarginPreset::Custom) {
        d->marginLeftSpin->setValue(std::get<0>(margins));
        d->marginRightSpin->setValue(std::get<1>(margins));
        d->marginTopSpin->setValue(std::get<2>(margins));
        d->marginBottomSpin->setValue(std::get<3>(margins));
    }
    
    d->isSettingFromPreset = false;
    d->updatePreview();
}

/**
 * @brief 处理自定义尺寸变化
 */
void PageSetupDialog::onCustomSizeChanged()
{
    if (d->isSettingFromPreset) {
        return;
    }
    
    d->isSettingFromPreset = true;
    d->pageSizeCombo->setCurrentIndex(d->pageSizeCombo->findData(QVariant::fromValue(PageSizePreset::Custom)));
    d->isSettingFromPreset = false;
    
    d->updatePreview();
}

/**
 * @brief 处理自定义边距变化
 */
void PageSetupDialog::onCustomMarginChanged()
{
    if (d->isSettingFromPreset) {
        return;
    }
    
    d->isSettingFromPreset = true;
    d->marginPresetCombo->setCurrentIndex(d->marginPresetCombo->findData(QVariant::fromValue(MarginPreset::Custom)));
    d->isSettingFromPreset = false;
    
    d->updatePreview();
}

/**
 * @brief 静态便利方法获取页面设置
 * @param initial 初始页面设置
 * @param parent 父窗口
 * @return 用户选择的页面设置
 */
PageSetup PageSetupDialog::getPageSetup(const PageSetup &initial, QWidget *parent)
{
    PageSetupDialog dialog(parent);
    dialog.d->initialSetup = initial;
    
    // 设置初始值
    dialog.d->isSettingFromPreset = true;
    
    dialog.d->pageWidthSpin->setValue(initial.pageWidth);
    dialog.d->pageHeightSpin->setValue(initial.pageHeight);
    dialog.d->marginLeftSpin->setValue(initial.marginLeft);
    dialog.d->marginRightSpin->setValue(initial.marginRight);
    dialog.d->marginTopSpin->setValue(initial.marginTop);
    dialog.d->marginBottomSpin->setValue(initial.marginBottom);
    
    if (initial.portrait) {
        dialog.d->portraitRadio->setChecked(true);
    } else {
        dialog.d->landscapeRadio->setChecked(true);
    }
    
    dialog.d->isSettingFromPreset = false;
    dialog.d->updatePreview();
    
    // 显示对话框
    if (dialog.exec() == QDialog::Accepted) {
        PageSetup result = dialog.selectedSetup();
        return result;
    }
    
    return initial;
}

/**
 * @brief 获取用户选择的设置
 * @return 当前选中的页面设置
 */
PageSetup PageSetupDialog::selectedSetup() const
{
    PageSetup setup;
    setup.pageWidth = d->pageWidthSpin->value();
    setup.pageHeight = d->pageHeightSpin->value();
    setup.marginLeft = d->marginLeftSpin->value();
    setup.marginRight = d->marginRightSpin->value();
    setup.marginTop = d->marginTopSpin->value();
    setup.marginBottom = d->marginBottomSpin->value();
    setup.portrait = d->portraitRadio->isChecked();
    return setup;
}

} // namespace QtWordEditor
