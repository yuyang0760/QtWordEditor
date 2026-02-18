#ifndef FORMATCONTROLLER_H
#define FORMATCONTROLLER_H

#include <QObject>
#include "core/document/CharacterStyle.h"
#include "core/document/ParagraphStyle.h"
#include "core/Global.h"

namespace QtWordEditor {

class Document;
class Selection;

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
     * @param selection 可选的选择区域对象
     * @param parent 父对象指针，默认为nullptr
     */
    explicit FormatController(Document *document, Selection *selection = nullptr,
                              QObject *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~FormatController() override;

    // ========== 字符格式化方法 ==========
    
    /**
     * @brief 应用完整的字符样式
     * @param style 要应用的字符样式对象
     */
    void applyCharacterStyle(const CharacterStyle &style);
    
    /**
     * @brief 设置字体
     * @param font 新的字体设置
     */
    void setFont(const QFont &font);
    
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

    // ========== 段落格式化方法 ==========
    
    /**
     * @brief 应用完整的段落样式
     * @param style 要应用的段落样式对象
     */
    void applyParagraphStyle(const ParagraphStyle &style);
    
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

private:
    Document *m_document;    ///< 关联的文档对象
    Selection *m_selection;  ///< 关联的选择区域对象
};

} // namespace QtWordEditor

#endif // FORMATCONTROLLER_H