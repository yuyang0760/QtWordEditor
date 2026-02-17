#ifndef PDFEXPORTER_H
#define PDFEXPORTER_H

#include <QString>
#include "core/Global.h"

namespace QtWordEditor {

class Document;

/**
 * @brief The PdfExporter class exports documents to PDF format.
 */
class PdfExporter
{
public:
    PdfExporter();
    ~PdfExporter();

    /**
     * @brief Export document to PDF file.
     * @param doc Document to export
     * @param filePath Output PDF file path
     * @return true on success, false on failure
     */
    bool exportToPdf(Document *doc, const QString &filePath);

    /**
     * @brief Get the last error message.
     */
    QString lastError() const;

private:
    QString m_lastError;
};

} // namespace QtWordEditor

#endif // PDFEXPORTER_H