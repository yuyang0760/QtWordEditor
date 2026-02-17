#ifndef XMLSERIALIZER_H
#define XMLSERIALIZER_H

#include <QString>
#include "core/Global.h"

namespace QtWordEditor {

class Document;

/**
 * @brief The XmlSerializer class serializes documents to/from XML format.
 */
class XmlSerializer
{
public:
    XmlSerializer();
    ~XmlSerializer();

    /**
     * @brief Serialize a document to XML file.
     * @param doc Document to serialize
     * @param filePath Output file path
     * @return true on success, false on failure
     */
    bool serialize(Document *doc, const QString &filePath);

    /**
     * @brief Deserialize a document from XML file.
     * @param filePath Input file path
     * @return Deserialized document (caller takes ownership), or nullptr on failure
     */
    Document *deserialize(const QString &filePath);

    /**
     * @brief Get the last error message.
     */
    QString lastError() const;

private:
    QString m_lastError;
};

} // namespace QtWordEditor

#endif // XMLSERIALIZER_H