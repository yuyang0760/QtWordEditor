#ifndef XMLSERIALIZER_H
#define XMLSERIALIZER_H

#include <QString>
#include "core/Global.h"

namespace QtWordEditor {

class Document;

/**
 * @brief XML序列化器类，负责文档与XML格式之间的序列化转换
 *
 * 该类提供文档的XML序列化和反序列化功能，支持：
 * 1. 将文档对象序列化为XML文件格式
 * 2. 从XML文件反序列化为文档对象
 * 3. 错误处理和错误信息获取
 * 4. 标准化的文档存储格式
 */
class XmlSerializer
{
public:
    /** @brief 默认构造函数 */
    XmlSerializer();
    
    /** @brief 析构函数 */
    ~XmlSerializer();

    /**
     * @brief 将文档序列化为XML文件
     * @param doc 要序列化的文档对象
     * @param filePath 输出文件路径
     * @return 成功返回true，失败返回false
     */
    bool serialize(Document *doc, const QString &filePath);

    /**
     * @brief 从XML文件反序列化文档
     * @param filePath 输入文件路径
     * @return 反序列化成功的文档对象（调用者拥有所有权），失败返回nullptr
     */
    Document *deserialize(const QString &filePath);

    /**
     * @brief 获取最后的错误信息
     * @return 错误信息字符串
     */
    QString lastError() const;

private:
    QString m_lastError;  ///< 最后一次操作的错误信息
};

} // namespace QtWordEditor

#endif // XMLSERIALIZER_H