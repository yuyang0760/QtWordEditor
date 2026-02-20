#ifndef REMOVETEXTCOMMAND_H
#define REMOVETEXTCOMMAND_H

#include "ParagraphCommand.h"
#include "core/document/CharacterStyle.h"
#include "core/document/Span.h"
#include <QString>
#include <QList>

namespace QtWordEditor {

/**
 * @brief 删除文本命令类，从指定块中删除一段文本
 *
 * 该命令负责从文档的特定块中删除指定范围的文本内容，
 * 并保存被删除的文本和样式信息以便撤销操作。
 */
class RemoveTextCommand : public ParagraphCommand
{
public:
    /**
     * @brief 构造函数
     * @param document 目标文档
     * @param blockIndex 目标块的索引
     * @param position 删除起始位置
     * @param length 要删除的文本长度
     */
    RemoveTextCommand(Document *document, int blockIndex, int position, int length);
    
    /**
     * @brief 析构函数
     */
    ~RemoveTextCommand() override;

    /**
     * @brief 执行重做操作
     * 删除指定范围的文本
     */
    void redo() override;
    
    /**
     * @brief 执行撤销操作
     * 恢复被删除的文本和样式
     */
    void undo() override;

private:
    int m_position;             ///< 删除起始位置
    int m_length;               ///< 删除的文本长度
    QString m_removedText;      ///< 被删除的文本内容
    QList<Span> m_removedSpans; ///< 被删除文本的样式跨度信息
};

} // namespace QtWordEditor

#endif // REMOVETEXTCOMMAND_H