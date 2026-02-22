#ifndef RIBBONBAR_H
#define RIBBONBAR_H

#include <QTabWidget>
#include <QHash>
#include <QAction>
#include <QFontComboBox>
#include <QComboBox>
#include "core/Global.h"
#include "core/document/CharacterStyle.h"

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
     * @brief 根据字符样式更新控件状态
     * @param style 要显示的字符样式对象
     */
    void updateFromSelection(const CharacterStyle &style);
    
    /**
     * @brief 各样式属性的一致性状态
     */
    struct StyleConsistency {
        bool fontFamilyConsistent;   // 字体是否一致
        bool fontSizeConsistent;     // 字号是否一致
        bool boldConsistent;         // 粗体是否一致
        bool italicConsistent;       // 斜体是否一致
        bool underlineConsistent;    // 下划线是否一致
        
        // 一致时的属性值
        QString consistentFontFamily;
        int consistentFontSize;
        bool consistentBold;
        bool consistentItalic;
        bool consistentUnderline;
        
        StyleConsistency() 
            : fontFamilyConsistent(true)
            , fontSizeConsistent(true)
            , boldConsistent(true)
            , italicConsistent(true)
            , underlineConsistent(true)
            , consistentFontFamily("")
            , consistentFontSize(0)
            , consistentBold(false)
            , consistentItalic(false)
            , consistentUnderline(false)
        {}
        
        StyleConsistency(bool allConsistent) 
            : fontFamilyConsistent(allConsistent)
            , fontSizeConsistent(allConsistent)
            , boldConsistent(allConsistent)
            , italicConsistent(allConsistent)
            , underlineConsistent(allConsistent)
            , consistentFontFamily("")
            , consistentFontSize(0)
            , consistentBold(false)
            , consistentItalic(false)
            , consistentUnderline(false)
        {}
    };

    /**
     * @brief 根据字符样式和样式一致性更新控件状态（向后兼容）
     * @param style 要显示的字符样式对象
     * @param styleConsistent true=样式一致（显示样式），false=样式不一致（不显示样式/混合状态）
     */
    void updateFromSelection(const CharacterStyle &style, bool styleConsistent);
    
    /**
     * @brief 根据字符样式和各属性的一致性状态更新控件状态
     * @param style 要显示的字符样式对象
     * @param consistency 各属性的一致性状态
     */
    void updateFromSelection(const CharacterStyle &style, const StyleConsistency &consistency);
    
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
    void insertFormulaRequested();
    void insertFractionRequested();
    void insertNumberRequested();
    void insertRadicalRequested();
    void insertSubscriptRequested();
    void insertSuperscriptRequested();

private:
    QScopedPointer<RibbonBarPrivate> d;
};

} // namespace QtWordEditor

#endif // RIBBONBAR_H
