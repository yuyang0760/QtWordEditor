#ifndef PDFEXPORTER_H
#define PDFEXPORTER_H

#include <QString>
#include "core/Global.h"

namespace QtWordEditor {

class Document;

/**
 * @brief PDF导出器类，负责将文档导出为PDF格式
 *
 * 该类提供文档到PDF的导出功能，支持：
 * 1. 高质量的PDF文档生成
 * 2. 保持原始文档的格式和布局
 * 3. 错误处理和状态反馈
 * 4. 标准PDF输出格式
 */
class PdfExporter
{
public:
    /** @brief 默认构造函数 */
    PdfExporter();
    
    /** @brief 析构函数 */
    ~PdfExporter();

    /**
     * @brief 将文档导出为PDF文件
     * @param doc 要导出的文档对象
     * @param filePath 输出PDF文件路径
     * @return 成功返回true，失败返回false
     */
    bool exportToPdf(Document *doc, const QString &filePath);

    /**
     * @brief 获取最后的错误信息
     * @return 错误信息字符串
     */
    QString lastError() const;

private:
    QString m_lastError;  ///< 最后一次导出操作的错误信息
};

} // namespace QtWordEditor

#endif // PDFEXPORTER_H