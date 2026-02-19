#ifndef FORMATCONTROLLER_H
#define FORMATCONTROLLER_H

#include <QObject>
#include <functional>
#include "core/document/CharacterStyle.h"
#include "core/document/ParagraphStyle.h"
#include "core/Global.h"

namespace QtWordEditor {

class Document;
class Selection;
class Cursor;
class StyleManager;
class ParagraphBlock;
struct CursorPosition;

/**
 * @brief 格式控制器类，提供高级格式化API接口
 *
 * 该类作为格式化操作的统一入口，负责：
 * 1. 字符级别格式设置（字体、颜色、样式等）
 * 2. 段落级别格式设置（对齐、缩进、行距等）
 * 3. 协调文档和选择区域的格式应用
 * 4. 提供便捷的格式化方法
 */
class FormatController : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param document 关联的文档对象
     * @param cursor 可选的光标对象
     * @param selection 可选的选择区域对象
     * @param styleManager 可选的样式管理器对象
     * @param parent 父对象指针，默认为nullptr
     */
    explicit FormatController(Document *document, Cursor *cursor = nullptr,
                              Selection *selection = nullptr,
                              StyleManager *styleManager = nullptr,
                              QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~FormatController() override;

    // ========== 字符格式化方法 ==========
    
    /**
     * @brief 应用完整的字符样式（使用 mergeWith 合并）
     * @param style 要应用的字符样式对象
     */
    void applyCharacterStyle(const CharacterStyle &style);
    
    /**
     * @brief 应用命名的字符样式
     * @param styleName 样式名称
     */
    void applyNamedCharacterStyle(const QString &styleName);
    
    /**
     * @brief 设置字体
     * @param font 新的字体设置
     */
    void setFont(const QFont &font);
    
    /**
     * @brief 设置字体族
     * @param family 字体族名称
     */
    void setFontFamily(const QString &family);
    
    /**
     * @brief 设置字体大小
     * @param size 字体大小（磅值）
     */
    void setFontSize(int size);
    
    /**
     * @brief 设置粗体
     * @param bold 是否启用粗体
     */
    void setBold(bool bold);
    
    /**
     * @brief 设置斜体
     * @param italic 是否启用斜体
     */
    void setItalic(bool italic);
    
    /**
     * @brief 设置下划线
     * @param underline 是否启用下划线
     */
    void setUnderline(bool underline);
    
    /**
     * @brief 设置文字颜色
     * @param color 新的文字颜色
     */
    void setTextColor(const QColor &color);
    
    /**
     * @brief 设置背景颜色
     * @param color 新的背景颜色
     */
    void setBackgroundColor(const QColor &color);
    
    /**
     * @brief 获取当前选区内的字符样式
     * @return 选区内的字符样式
     */
    CharacterStyle getCurrentCharacterStyle() const;

    // ========== 样式获取方法（用于工具栏显示） ==========

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
    };

    /**
     * @brief 检查选区各样式属性是否一致
     * 
     * @return 各属性的一致性状态
     */
    StyleConsistency getSelectionStyleConsistency() const;

    /**
     * @brief 检查选区是否样式一致（用于向后兼容）
     * 
     * @return true=所有属性都一致，false=至少有一个属性不一致
     */
    bool isSelectionStyleConsistent() const;
    
    /**
     * @brief 检查选区内的所有字符的粗体是否都为 true
     * 
     * @return true=选区内所有字符都是粗体，false=至少有一个字符不是粗体
     */
    bool isSelectionAllBold() const;
    
    /**
     * @brief 检查选区内的所有字符的斜体是否都为 true
     * 
     * @return true=选区内所有字符都是斜体，false=至少有一个字符不是斜体
     */
    bool isSelectionAllItalic() const;
    
    /**
     * @brief 检查选区内的所有字符的下划线是否都为 true
     * 
     * @return true=选区内所有字符都有下划线，false=至少有一个字符没有下划线
     */
    bool isSelectionAllUnderline() const;

    /**
     * @brief 获取当前应该在工具栏显示的样式
     * 
     * 逻辑：
     * - 有选区：返回选区终点（Focus）的前一个字符的样式
     * - 无选区：返回光标前一个字符的样式
     * 
     * @return 应该显示的字符样式
     */
    CharacterStyle getCurrentDisplayStyle() const;

    /**
     * @brief 获取选区的终点位置（Focus）
     * 
     * @return 选区终点的位置 {blockIndex, offset}
     */
    CursorPosition getSelectionEndPosition() const;

    /**
     * @brief 获取指定位置的字符样式
     * 
     * @param blockIndex 块索引
     * @param offset 块内偏移量
     * @return 该位置的字符样式
     */
    CharacterStyle getStyleAtPosition(int blockIndex, int offset) const;

    // ========== 段落格式化方法 ==========
    
    /**
     * @brief 应用完整的段落样式（使用 mergeWith 合并）
     * @param style 要应用的段落样式对象
     */
    void applyParagraphStyle(const ParagraphStyle &style);
    
    /**
     * @brief 应用命名的段落样式
     * @param styleName 样式名称
     */
    void applyNamedParagraphStyle(const QString &styleName);
    
    /**
     * @brief 设置段落对齐方式
     * @param align 对齐方式枚举值
     */
    void setAlignment(ParagraphAlignment align);
    
    /**
     * @brief 设置左缩进
     * @param indent 左缩进值
     */
    void setLeftIndent(qreal indent);
    
    /**
     * @brief 设置右缩进
     * @param indent 右缩进值
     */
    void setRightIndent(qreal indent);
    
    /**
     * @brief 设置首行缩进
     * @param indent 首行缩进值
     */
    void setFirstLineIndent(qreal indent);
    
    /**
     * @brief 设置行高百分比
     * @param percent 行高百分比（如150表示1.5倍行距）
     */
    void setLineHeight(int percent);
    
    /**
     * @brief 设置段前间距
     * @param space 段前间距值
     */
    void setSpaceBefore(qreal space);
    
    /**
     * @brief 设置段后间距
     * @param space 段后间距值
     */
    void setSpaceAfter(qreal space);
    
    /**
     * @brief 获取当前选区内的段落样式
     * @return 选区内的段落样式
     */
    ParagraphStyle getCurrentParagraphStyle() const;

    // ========== 当前输入样式方法 ==========

    /**
     * @brief 获取当前输入样式（用于打字时使用）
     * @return 当前应该使用的输入样式
     */
    CharacterStyle getCurrentInputStyle() const;

    /**
     * @brief 设置当前输入样式（用户通过工具栏手动设置样式时调用）
     * @param style 用户设置的样式
     */
    void setCurrentInputStyle(const CharacterStyle &style);

    /**
     * @brief 光标移动时调用，更新当前输入样式（继承模式）
     */
    void onCursorMoved();

private:
    // ========== 辅助方法 ==========
    
    /**
     * @brief 收集选区内所有 Span 的样式
     * @return Span 样式列表
     */
    QList<CharacterStyle> collectSelectionStyles() const;
    
    /**
     * @brief 检查选区内所有样式是否都满足某个条件
     * @param checkFunc 检查函数
     * @return true=所有样式都满足条件
     */
    bool checkSelectionAll(const std::function<bool(const CharacterStyle&)>& checkFunc) const;
    
    /**
     * @brief 应用单个字符样式属性
     * @param setPropertyFunc 设置属性的函数
     */
    void applySingleProperty(const std::function<void(CharacterStyle&)>& setPropertyFunc);
    
    /**
     * @brief 获取指定块的 ParagraphBlock
     * @param blockIndex 块索引
     * @return ParagraphBlock 指针，失败返回 nullptr
     */
    ParagraphBlock* getParagraphBlock(int blockIndex) const;
    
    /**
     * @brief 验证选区有效性
     * @return true=选区有效
     */
    bool validateSelection() const;
    
    /**
     * @brief 应用单个段落样式属性
     * @param setPropertyFunc 设置属性的函数
     */
    void applySingleParagraphProperty(const std::function<void(ParagraphStyle&)>& setPropertyFunc);

    // ========== 成员变量 ==========
    Document *m_document;       ///< 关联的文档对象
    Cursor *m_cursor;           ///< 关联的光标对象
    Selection *m_selection;     ///< 关联的选择区域对象
    StyleManager *m_styleManager; ///< 关联的样式管理器对象
    CharacterStyle m_currentInputStyle;  ///< 当前输入样式
    bool m_isInputStyleOverridden;       ///< 是否处于覆盖模式（用户手动设置过样式）
};

} // namespace QtWordEditor

#endif // FORMATCONTROLLER_H