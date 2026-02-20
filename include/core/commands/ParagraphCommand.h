/**
 * @file ParagraphCommand.h
 * @brief 段落相关命令的基类
 *
 * 本文件定义了ParagraphCommand类，作为所有操作段落块的命令的基类，
 * 提供通用的段落块获取和类型检查功能，减少重复代码。
 */

#ifndef PARAGRAPHCOMMAND_H
#define PARAGRAPHCOMMAND_H

#include "core/commands/EditCommand.h"
#include "core/document/ParagraphBlock.h"

namespace QtWordEditor {

/**
 * @brief 段落相关命令的基类
 *
 * 提供通用的段落块获取和类型检查功能，减少重复代码。
 */
class ParagraphCommand : public EditCommand
{
public:
    /**
     * @brief 构造ParagraphCommand对象
     * @param document 要操作的文档
     * @param blockIndex 段落块的全局索引
     * @param text 命令文本
     */
    ParagraphCommand(Document *document, int blockIndex, const QString &text);
    
    /**
     * @brief 析构ParagraphCommand对象
     */
    virtual ~ParagraphCommand();
    
protected:
    /**
     * @brief 获取段落块
     * @return 段落块指针，如果获取失败返回nullptr
     */
    ParagraphBlock *getParagraphBlock() const;
    
    /**
     * @brief 获取块索引
     * @return 块索引
     */
    int blockIndex() const;
    
private:
    int m_blockIndex; ///< 段落块的全局索引
};

} // namespace QtWordEditor

#endif // PARAGRAPHCOMMAND_H
