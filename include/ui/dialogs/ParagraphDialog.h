#ifndef PARAGRAPHDIALOG_H
#define PARAGRAPHDIALOG_H

#include <QDialog>
#include "core/document/ParagraphStyle.h"
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief 段落设置对话框类，用于设置段落样式
 *
 * 该对话框提供用户友好的界面来设置段落的各种属性：
 * 1. 对齐方式（左对齐、居中、右对齐、两端对齐、分散对齐）
 * 2. 缩进设置（首行缩进、左右缩进）
 * 3. 间距设置（段前段后间距、行高）
 * 4. 实时预览功能
 * 5. 应用与取消机制
 * 支持静态方法快速获取段落样式设置
 */
class ParagraphDialog : public QDialog
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口部件指针，默认为nullptr
     */
    explicit ParagraphDialog(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~ParagraphDialog() override;

    /**
     * @brief 静态便利方法获取段落样式
     * @param initial 初始段落样式
     * @param parent 父窗口部件指针
     * @return 用户设置的段落样式
     */
    static ParagraphStyle getStyle(const ParagraphStyle &initial, QWidget *parent = nullptr);

    /**
     * @brief 设置初始段落样式
     * @param style 初始段落样式
     */
    void setInitialStyle(const ParagraphStyle &style);

    /**
     * @brief 获取用户选择的样式
     * @return 当前选中的段落样式
     */
    ParagraphStyle selectedStyle() const;

signals:
    /**
     * @brief 应用按钮被点击时发出的信号
     */
    void applied();

private:
    /**
     * @brief 更新当前样式从控件值
     */
    void updateCurrentStyle();

private:
    class Private;              ///< 私有实现类
    QScopedPointer<Private> d;  ///< 私有实现指针
};

} // namespace QtWordEditor

#endif // PARAGRAPHDIALOG_H
