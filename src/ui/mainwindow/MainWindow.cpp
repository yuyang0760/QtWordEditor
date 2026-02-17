#include "ui/mainwindow/MainWindow.h"
#include "core/document/Document.h"
#include "core/document/Section.h"
#include "core/document/ParagraphBlock.h"
#include "graphics/scene/DocumentScene.h"
#include "graphics/view/DocumentView.h"
#include "editcontrol/cursor/Cursor.h"
#include "editcontrol/selection/Selection.h"
#include "editcontrol/handlers/EditEventHandler.h"
#include "editcontrol/formatting/FormatController.h"
#include "core/styles/StyleManager.h"
#include "ui/widgets/FormatToolBar.h"
#include "ui/ribbon/RibbonBar.h"
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QCloseEvent>
#include <QDebug>

namespace QtWordEditor {

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_document(nullptr)
    , m_scene(nullptr)
    , m_view(nullptr)
    , m_cursor(nullptr)
    , m_selection(nullptr)
    , m_editEventHandler(nullptr)
    , m_formatController(nullptr)
    , m_styleManager(nullptr)
    , m_formatToolBar(nullptr)
    , m_ribbonBar(nullptr)
    , m_isModified(false)
{
    setupUi();
    createActions();
    newDocument(); // start with an empty document
    updateWindowTitle();
}

MainWindow::~MainWindow()
{
}

Document *MainWindow::document() const
{
    return m_document;
}

void MainWindow::setDocument(Document *document)
{
    if (m_document == document)
        return;

    // Clean up old document
    if (m_document) {
        // Disconnect signals
    }

    m_document = document;
    m_scene->setDocument(document);
    // m_cursor and m_selection are created elsewhere and passed to controllers
    m_styleManager->setDocument(document);

    // Update UI
    // ...
}

void MainWindow::setupUi()
{
    // Create central widget and layout
    m_view = new DocumentView(this);
    m_scene = new DocumentScene(this);
    m_view->setScene(m_scene);
    setCentralWidget(m_view);

    // Create core objects
    m_document = new Document(this);
    m_cursor = new Cursor(m_document, this);
    m_selection = new Selection(m_document, this);
    m_editEventHandler = new EditEventHandler(m_document, m_cursor, m_selection, this);
    m_formatController = new FormatController(m_document, m_selection, this);
    m_styleManager = new StyleManager(this);

    // Connect signals
    connect(m_view, &DocumentView::keyPressed,
            m_editEventHandler, &EditEventHandler::handleKeyPress);
    // Connect view's mouse events to event handler
    connect(m_view, &DocumentView::mousePressed,
            m_editEventHandler, &EditEventHandler::handleMousePress);
    connect(m_view, &DocumentView::mouseMoved,
            m_editEventHandler, &EditEventHandler::handleMouseMove);
    connect(m_view, &DocumentView::mouseReleased,
            m_editEventHandler, &EditEventHandler::handleMouseRelease);

    // Connect document modification signal to update window title
    connect(m_document, &Document::documentChanged,
            this, &MainWindow::updateWindowTitle);

    // Connect cursor position changes to update UI
    connect(m_cursor, &Cursor::positionChanged,
            this, &MainWindow::updateUI);

    // Create toolbar and status bar
    m_formatToolBar = new FormatToolBar(this);
    addToolBar(Qt::TopToolBarArea, m_formatToolBar);

    // Create ribbon bar (placeholder)
    m_ribbonBar = new RibbonBar(this);
    // For now, add ribbon as a top-level widget (could be integrated into main window layout)
    // TODO: integrate ribbon properly

    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createActions()
{
    // File menu
    QAction *newAct = new QAction(tr("&New"), this);
    newAct->setShortcut(QKeySequence::New);
    connect(newAct, &QAction::triggered, this, &MainWindow::newDocument);

    QAction *openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcut(QKeySequence::Open);
    connect(openAct, &QAction::triggered, this, &MainWindow::openDocument);

    QAction *saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcut(QKeySequence::Save);
    connect(saveAct, &QAction::triggered, this, &MainWindow::saveDocument);

    QAction *saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcut(QKeySequence::SaveAs);
    connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAsDocument);

    QAction *exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(QKeySequence::Quit);
    connect(exitAct, &QAction::triggered, this, &QWidget::close);

    // Edit menu
    QAction *undoAct = new QAction(tr("&Undo"), this);
    undoAct->setShortcut(QKeySequence::Undo);
    connect(undoAct, &QAction::triggered, this, &MainWindow::undo);

    QAction *redoAct = new QAction(tr("&Redo"), this);
    redoAct->setShortcut(QKeySequence::Redo);
    connect(redoAct, &QAction::triggered, this, &MainWindow::redo);

    QAction *cutAct = new QAction(tr("Cu&t"), this);
    cutAct->setShortcut(QKeySequence::Cut);
    connect(cutAct, &QAction::triggered, this, &MainWindow::cut);

    QAction *copyAct = new QAction(tr("&Copy"), this);
    copyAct->setShortcut(QKeySequence::Copy);
    connect(copyAct, &QAction::triggered, this, &MainWindow::copy);

    QAction *pasteAct = new QAction(tr("&Paste"), this);
    pasteAct->setShortcut(QKeySequence::Paste);
    connect(pasteAct, &QAction::triggered, this, &MainWindow::paste);

    // Format menu
    QAction *fontAct = new QAction(tr("&Font..."), this);
    connect(fontAct, &QAction::triggered, this, [this]() {
        // TODO: open font dialog and apply style
        qDebug() << "Font dialog placeholder";
    });

    QAction *paragraphAct = new QAction(tr("&Paragraph..."), this);
    connect(paragraphAct, &QAction::triggered, this, [this]() {
        // TODO: open paragraph dialog and apply style
        qDebug() << "Paragraph dialog placeholder";
    });

    // Insert menu
    QAction *imageAct = new QAction(tr("&Image..."), this);
    connect(imageAct, &QAction::triggered, this, [this]() {
        // TODO: open insert image dialog
        qDebug() << "Insert image dialog placeholder";
    });

    // View menu
    QAction *zoomInAct = new QAction(tr("Zoom &In"), this);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    connect(zoomInAct, &QAction::triggered, this, &MainWindow::zoomIn);

    QAction *zoomOutAct = new QAction(tr("Zoom &Out"), this);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    connect(zoomOutAct, &QAction::triggered, this, &MainWindow::zoomOut);

    QAction *zoomToFitAct = new QAction(tr("Zoom to &Fit"), this);
    connect(zoomToFitAct, &QAction::triggered, this, &MainWindow::zoomToFit);

    // Help menu
    QAction *aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);

    // Add actions to menus
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    QMenu *formatMenu = menuBar()->addMenu(tr("F&ormat"));
    formatMenu->addAction(fontAct);
    formatMenu->addAction(paragraphAct);

    QMenu *insertMenu = menuBar()->addMenu(tr("&Insert"));
    insertMenu->addAction(imageAct);

    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(zoomInAct);
    viewMenu->addAction(zoomOutAct);
    viewMenu->addAction(zoomToFitAct);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
}

bool MainWindow::maybeSave()
{
    if (!m_isModified)
        return true;
    QMessageBox::StandardButton ret = QMessageBox::warning(this, tr("Application"),
         tr("The document has been modified.\nDo you want to save your changes?"),
         QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (ret == QMessageBox::Save)
        return saveDocument();
    else if (ret == QMessageBox::Cancel)
        return false;
    return true;
}

void MainWindow::newDocument()
{
    if (maybeSave()) {
        delete m_document;
        m_document = new Document(this);
        setDocument(m_document);
        m_currentFile.clear();
        m_isModified = false;
        statusBar()->showMessage(tr("New document created"));
    }
}

void MainWindow::openDocument()
{
    if (!maybeSave())
        return;
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Document"), "", tr("QtWord Documents (*.qtdoc);;All Files (*)"));
    if (fileName.isEmpty())
        return;

    // TODO: load document via IO module
    // For now, just create an empty document
    newDocument();
    m_currentFile = fileName;
    m_isModified = false;
    statusBar()->showMessage(tr("Loaded %1").arg(fileName));
}

bool MainWindow::saveDocument()
{
    if (m_currentFile.isEmpty())
        return saveAsDocument();
    // TODO: save via IO module
    m_isModified = false;
    statusBar()->showMessage(tr("Saved %1").arg(m_currentFile));
    return true;
}

bool MainWindow::saveAsDocument()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Document As"), "", tr("QtWord Documents (*.qtdoc);;All Files (*)"));
    if (fileName.isEmpty())
        return false;
    m_currentFile = fileName;
    return saveDocument();
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About QtWordEditor"),
        tr("QtWordEditor is a rich‑text word processor built with Qt and C++17."));
}

void MainWindow::undo()
{
    if (m_document && m_document->undoStack())
        m_document->undoStack()->undo();
}

void MainWindow::redo()
{
    if (m_document && m_document->undoStack())
        m_document->undoStack()->redo();
}

void MainWindow::cut()
{
    // TODO: implement cut (copy + delete)
}

void MainWindow::copy()
{
    // TODO: implement copy to clipboard
}

void MainWindow::paste()
{
    // TODO: implement paste from clipboard
}

void MainWindow::zoomIn()
{
    m_view->zoomIn();
}

void MainWindow::zoomOut()
{
    m_view->zoomOut();
}

void MainWindow::zoomToFit()
{
    m_view->zoomToFit();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::updateWindowTitle()
{
    QString title = tr("QtWordEditor");
    if (!m_currentFile.isEmpty()) {
        title = QFileInfo(m_currentFile).fileName() + " - " + title;
    }
    if (m_isModified) {
        title = "*" + title;
    }
    setWindowTitle(title);
}

void MainWindow::updateUI()
{
    // Update toolbar and ribbon bar based on current selection
    if (m_formatToolBar) {
        m_formatToolBar->updateFromSelection();
    }
    if (m_ribbonBar) {
        m_ribbonBar->updateFromSelection();
    }
}

} // namespace QtWordEditor