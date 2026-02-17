#ifndef TXTIMPORTER_H
#define TXTIMPORTER_H

#include <QString>
#include "core/Global.h"

namespace QtWordEditor {

class Document;

/**
 * @brief The TxtImporter class imports plain text files into documents.
 */
class TxtImporter
{
public:
    TxtImporter();
    ~TxtImporter();

    /**
     * @brief Import plain text file into a document.
     * @param filePath Input text file path
     * @return Imported document (caller takes ownership), or nullptr on failure
     */
    Document *importFromTxt(const QString &filePath);

    /**
     * @brief Get the last error message.
     */
    QString lastError() const;

private:
    QString m_lastError;
};

} // namespace QtWordEditor

#endif // TXTIMPORTER_H