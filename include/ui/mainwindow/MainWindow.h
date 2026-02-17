#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "core/Global.h"
#include "ui/dialogs/PageSetupDialog.h"

namespace QtWordEditor {

class Document;
class DocumentScene;
class DocumentView;
class Cursor;
class Selection;
class EditEventHandler;
class FormatController;
class StyleManager;
class RibbonBar;

/**
 * @brief The MainWindow class is the application's main window.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    Document *document() const;
    void setDocument(Document *document);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void newDocument();
    void openDocument();
    bool saveDocument();
    bool saveAsDocument();
    void about();

    void undo();
    void redo();
    void cut();
    void copy();
    void paste();

    void zoomIn();
    void zoomOut();
    void zoomToFit();
    void pageSetup();
    void updateWindowTitle();
    void updateUI();
    void updateStatusBar(const QPointF &scenePos, const QPoint &viewPos);
    void switchToChinese();
    void switchToEnglish();

private:
    void setupUi();
    void createActions();
    bool maybeSave();
    void retranslateUi();

    Document *m_document;
    DocumentScene *m_scene;
    DocumentView *m_view;
    Cursor *m_cursor;
    Selection *m_selection;
    EditEventHandler *m_editEventHandler;
    FormatController *m_formatController;
    StyleManager *m_styleManager;
    RibbonBar *m_ribbonBar;

    QString m_currentFile;
    bool m_isModified;
    qreal m_currentZoom;
    QPointF m_lastScenePos;
    QPoint m_lastViewPos;
    PageSetup m_pageSetup;
};

} // namespace QtWordEditor

#endif // MAINWINDOW_H
