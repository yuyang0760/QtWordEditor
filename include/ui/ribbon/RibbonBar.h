#ifndef RIBBONBAR_H
#define RIBBONBAR_H

#include <QTabWidget>
#include <QHash>
#include <QAction>
#include <QFontComboBox>
#include <QSpinBox>
#include "core/Global.h"

namespace QtWordEditor {

class RibbonBarPrivate;
class StyleManager;

/**
 * @brief The RibbonBar class provides a Ribbon-style interface similar to Microsoft Office.
 *
 * This is a simplified implementation that uses QTabWidget as base.
 * For a production application, consider using a dedicated Ribbon library.
 */
class RibbonBar : public QTabWidget
{
    Q_OBJECT
public:
    explicit RibbonBar(StyleManager *styleManager, QWidget *parent = nullptr);
    ~RibbonBar() override;

    int addTab(const QString &title);
    void addGroup(const QString &groupName);
    void addAction(QAction *action);
    void addWidget(QWidget *widget);
    
    /**
     * @brief 根据当前选择更新控件状态
     * @param characterStyleName 当前字符样式名称
     * @param paragraphStyleName 当前段落样式名称
     */
    void updateFromSelection(const QString &characterStyleName = QString(),
                            const QString &paragraphStyleName = QString());
    
    /**
     * @brief 刷新样式列表
     * 当样式发生变化时调用
     */
    void refreshStyleLists();

signals:
    void fontChanged(const QFont &font);
    void fontSizeChanged(int size);
    void boldChanged(bool bold);
    void italicChanged(bool italic);
    void underlineChanged(bool underline);
    void alignmentChanged(Qt::Alignment alignment);
    void characterStyleChanged(const QString &styleName);
    void paragraphStyleChanged(const QString &styleName);
    void openStyleManagerRequested();

private:
    QScopedPointer<RibbonBarPrivate> d;
};

} // namespace QtWordEditor

#endif // RIBBONBAR_H
