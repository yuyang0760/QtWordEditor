#include "TxtImporter.h"
#include "core/document/Document.h"

namespace QtWordEditor {

TxtImporter::TxtImporter()
{
}

TxtImporter::~TxtImporter()
{
}

Document *TxtImporter::importFromTxt(const QString &filePath)
{
    Q_UNUSED(filePath)
    m_lastError = "Not implemented";
    return nullptr;
}

QString TxtImporter::lastError() const
{
    return m_lastError;
}

} // namespace QtWordEditor