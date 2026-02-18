#ifndef PAGESETUPDIALOG_H
#define PAGESETUPDIALOG_H

#include <QDialog>
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief 页面设置结构体，定义页面的各种尺寸参数
 * 单位：毫米
 */
struct PageSetup
{
    qreal pageWidth = 210.0;    ///< 页面宽度（毫米）
    qreal pageHeight = 297.0;   ///< 页面高度（毫米）
    qreal marginLeft = 20.0;    ///< 左边距
    qreal marginRight = 20.0;   ///< 右边距
    qreal marginTop = 20.0;     ///< 上边距
    qreal marginBottom = 20.0;  ///< 下边距
    bool portrait = true;       ///< 是否纵向（true为纵向，false为横向）
};

/**
 * @brief 页面尺寸预设枚举
 * 定义常用的页面尺寸标准
 */
enum class PageSizePreset {
    A4,      ///< A4纸张 (210×297mm)
    A3,      ///< A3纸张 (297×420mm)
    A5,      ///< A5纸张 (148×210mm)
    Letter,  ///< Letter纸张 (8.5×11英寸)
    Legal,   ///< Legal纸张 (8.5×14英寸)
    Tabloid, ///< Tabloid纸张 (11×17英寸)
    Custom   ///< 自定义尺寸
};

/**
 * @brief 边距预设枚举
 * 定义常用的边距设置
 */
enum class MarginPreset {
    Normal,    ///< 标准边距
    Narrow,    ///< 窄边距
    Moderate,  ///< 中等边距
    Wide,      ///< 宽边距
    Custom     ///< 自定义边距
};

/**
 * @brief 页面设置对话框类，用于设置页面尺寸和边距
 *
 * 该对话框提供直观的界面让用户设置：
 * 1. 页面尺寸（支持多种标准纸张尺寸）
 * 2. 页面方向（纵向/横向）
 * 3. 边距设置（预设或自定义）
 * 4. 自定义页面尺寸
 */
class PageSetupDialog : public QDialog
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口部件指针，默认为nullptr
     */
    explicit PageSetupDialog(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~PageSetupDialog() override;

    /**
     * @brief 静态便利方法获取页面设置
     * @param initial 初始页面设置
     * @param parent 父窗口部件指针
     * @return 用户选择的页面设置
     */
    static PageSetup getPageSetup(const PageSetup &initial, QWidget *parent = nullptr);

    /**
     * @brief 获取用户选择的设置
     * @return 当前选中的页面设置
     */
    PageSetup selectedSetup() const;

private slots:
    /**
     * @brief 处理页面尺寸预设变化
     * @param index 新的预设索引
     */
    void onPageSizePresetChanged(int index);
    
    /** @brief 处理页面方向变化 */
    void onOrientationChanged();
    
    /**
     * @brief 处理边距预设变化
     * @param index 新的预设索引
     */
    void onMarginPresetChanged(int index);
    
    /** @brief 处理自定义尺寸变化 */
    void onCustomSizeChanged();
    
    /** @brief 处理自定义边距变化 */
    void onCustomMarginChanged();

private:
    class Private;                    ///< 私有实现类
    QScopedPointer<Private> d;        ///< 私有实现指针
};

} // namespace QtWordEditor

#endif // PAGESETUPDIALOG_H