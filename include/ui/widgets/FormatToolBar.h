#ifndef FORMATTOOLBAR_H
#define FORMATTOOLBAR_H

#include <QToolBar>
#include "core/Global.h"

namespace QtWordEditor {

class FormatToolBarPrivate;

/**
 * @brief 格式工具栏类，提供格式化控制的工具栏
 *
 * 该类是一个专门的工具栏，用于控制文档的格式设置，包括：
 * 1. 字体选择和设置
 * 2. 字号调整
 * 3. 文本样式（粗体、斜体、下划线）
 * 4. 文本对齐方式
 * 5. 根据当前选择动态更新控件状态
 */
class FormatToolBar : public QToolBar
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父窗口部件指针，默认为nullptr
     */
    explicit FormatToolBar(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~FormatToolBar() override;

    /**
     * @brief 根据当前选择更新控件状态
     * 例如更新粗体按钮的状态
     */
    void updateFromSelection();

public slots:
    /**
     * @brief 处理字体变化
     * @param font 新的字体设置
     */
    void onFontChanged(const QFont &font);
    
    /**
     * @brief 处理字号变化
     * @param size 新的字号
     */
    void onFontSizeChanged(int size);
    
    /**
     * @brief 处理粗体状态变化
     * @param bold 是否为粗体
     */
    void onBoldChanged(bool bold);
    
    /**
     * @brief 处理斜体状态变化
     * @param italic 是否为斜体
     */
    void onItalicChanged(bool italic);
    
    /**
     * @brief 处理下划线状态变化
     * @param underline 是否有下划线
     */
    void onUnderlineChanged(bool underline);
    
    /**
     * @brief 处理对齐方式变化
     * @param alignment 新的对齐方式
     */
    void onAlignmentChanged(Qt::Alignment alignment);

signals:
    /** @brief 字体发生变化时发出的信号 */
    void fontChanged(const QFont &font);
    
    /** @brief 字号发生变化时发出的信号 */
    void fontSizeChanged(int size);
    
    /** @brief 粗体状态发生变化时发出的信号 */
    void boldChanged(bool bold);
    
    /** @brief 斜体状态发生变化时发出的信号 */
    void italicChanged(bool italic);
    
    /** @brief 下划线状态发生变化时发出的信号 */
    void underlineChanged(bool underline);
    
    /** @brief 对齐方式发生变化时发出的信号 */
    void alignmentChanged(Qt::Alignment alignment);

private:
    QScopedPointer<FormatToolBarPrivate> d;  ///< 私有实现指针
};

} // namespace QtWordEditor

#endif // FORMATTOOLBAR_H