#ifndef INSERTTEXTCOMMAND_H
#define INSERTTEXTCOMMAND_H

#include "ParagraphCommand.h"
#include "core/document/CharacterStyle.h"
#include <QString>

namespace QtWordEditor {

/**
 * @brief 插入文本命令类，在指定块的指定位置插入文本
 *
 * 该命令负责在文档的特定块中插入文本内容，并支持撤销重做操作。
 * 支持与相邻的插入命令合并，提高撤销操作的用户体验。
 */
class InsertTextCommand : public ParagraphCommand
{
public:
    /**
     * @brief 构造函数
     * @param document 目标文档
     * @param blockIndex 目标块的索引
     * @param position 在块中的插入位置
     * @param text 要插入的文本内容
     * @param style 插入文本的字符样式
     */
    InsertTextCommand(Document *document, int blockIndex, int position,
                      const QString &text, const CharacterStyle &style);
    
    /**
     * @brief 析构函数
     */
    ~InsertTextCommand() override;

    /**
     * @brief 执行重做操作
     * 将文本插入到指定位置
     */
    void redo() override;
    
    /**
     * @brief 执行撤销操作
     * 删除刚刚插入的文本
     */
    void undo() override;

    /**
     * @brief 尝试与其他命令合并
     * 主要用于合并相邻的文本插入操作
     * @param other 要合并的另一个命令
     * @return 如果成功合并返回true，否则返回false
     */
    bool mergeWith(const QUndoCommand *other) override;

private:
    int m_position;             ///< 插入位置
    QString m_text;             ///< 插入的文本内容
    CharacterStyle m_style;     ///< 文本字符样式
};

} // namespace QtWordEditor

#endif // INSERTTEXTCOMMAND_H