#ifndef FONTDIALOG_H
#define FONTDIALOG_H

#include <QDialog>
#include "core/document/CharacterStyle.h"
#include "core/Global.h"

namespace QtWordEditor {

/**
 * @brief 字体对话框类，用于选择字符样式
 *
 * 该对话框扩展了QFontDialog的功能，增加了颜色选择支持：
 * 1. 字体选择（字体族、大小、样式）
 * 2. 颜色选择（文字颜色、背景颜色）
 * 3. 字符效果（粗体、斜体、下划线等）
 * 4. 提供静态方法快速获取字符样式设置
 */
class FontDialog : public QDialog
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口部件指针，默认为nullptr
     */
    explicit FontDialog(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~FontDialog() override;

    /**
     * @brief 静态便利方法获取字符样式
     * @param initial 初始字符样式
     * @param parent 父窗口部件指针
     * @return 用户设置的字符样式
     */
    static CharacterStyle getStyle(const CharacterStyle &initial, QWidget *parent = nullptr);

    /**
     * @brief 获取用户选择的样式
     * @return 当前选中的字符样式
     */
    CharacterStyle selectedStyle() const;

private slots:
    /**
     * @brief 处理字体选择变化
     * @param font 新选择的字体
     */
    void onFontSelected(const QFont &font);
    
    /** @brief 处理颜色选择点击 */
    void onColorClicked();

private:
    class Private;              ///< 私有实现类
    QScopedPointer<Private> d;  ///< 私有实现指针
};

} // namespace QtWordEditor

#endif // FONTDIALOG_H