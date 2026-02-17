#include "XmlSerializer.h"
#include "core/document/Document.h"

namespace QtWordEditor {

XmlSerializer::XmlSerializer()
{
}

XmlSerializer::~XmlSerializer()
{
}

bool XmlSerializer::serialize(Document *doc, const QString &filePath)
{
    Q_UNUSED(doc)
    Q_UNUSED(filePath)
    m_lastError = "Not implemented";
    return false;
}

Document *XmlSerializer::deserialize(const QString &filePath)
{
    Q_UNUSED(filePath)
    m_lastError = "Not implemented";
    return nullptr;
}

QString XmlSerializer::lastError() const
{
    return m_lastError;
}

} // namespace QtWordEditor