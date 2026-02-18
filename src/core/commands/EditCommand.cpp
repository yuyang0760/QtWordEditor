
/**
 * @file EditCommand.cpp
 * @brief 编辑命令基类的实现文件
 *
 * 此文件包含了编辑命令基类的实现，为应用程序中所有可撤销的编辑操作
 * 提供统一的基础框架和接口。
 */

#include "core/commands/EditCommand.h"
#include "core/document/Document.h"

namespace QtWordEditor {

/**
 * @brief Constructs an EditCommand object
 * @param document The document this command operates on
 * @param text Optional text describing the command (for undo menu)
 */
EditCommand::EditCommand(Document *document, const QString &amp;text)
    : QUndoCommand(text)
    , m_document(document)
{
}

/**
 * @brief 销毁编辑命令对象
 */
EditCommand::~EditCommand()
{
}

/**
 * @brief 获取此命令操作的文档
 * @return 指向文档对象的指针
 */
Document *EditCommand::document() const
{
    return m_document;
}

/**
 * @brief 撤销命令操作（需要由子类重写实现）
 * 默认实现为空，具体的撤销逻辑在子类中实现
 */
void EditCommand::undo()
{
}

/**
 * @brief 重做命令操作（需要由子类重写实现）
 * 默认实现为空，具体的重做逻辑在子类中实现
 */
void EditCommand::redo()
{
}

} // namespace QtWordEditor

