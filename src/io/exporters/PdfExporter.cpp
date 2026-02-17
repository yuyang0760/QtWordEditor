#include "io/exporters/PdfExporter.h"
#include "core/document/Document.h"

namespace QtWordEditor {

PdfExporter::PdfExporter()
{
}

PdfExporter::~PdfExporter()
{
}

bool PdfExporter::exportToPdf(Document *doc, const QString &filePath)
{
    Q_UNUSED(doc)
    Q_UNUSED(filePath)
    m_lastError = "Not implemented";
    return false;
}

QString PdfExporter::lastError() const
{
    return m_lastError;
}

} // namespace QtWordEditor