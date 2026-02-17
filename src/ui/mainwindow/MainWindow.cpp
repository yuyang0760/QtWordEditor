#include "ui/mainwindow/MainWindow.h"
#include "core/document/Document.h"
#include "core/document/Section.h"
#include "core/document/ParagraphBlock.h"
#include "core/document/Page.h"
#include "core/layout/PageBuilder.h"
#include "core/utils/Constants.h"
#include "graphics/scene/DocumentScene.h"
#include "graphics/view/DocumentView.h"
#include "editcontrol/cursor/Cursor.h"
#include "editcontrol/selection/Selection.h"
#include "editcontrol/handlers/EditEventHandler.h"
#include "editcontrol/formatting/FormatController.h"
#include "core/styles/StyleManager.h"
#include "ui/ribbon/RibbonBar.h"
#include "ui/dialogs/PageSetupDialog.h"
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QWidget>
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
    , m_ribbonBar(nullptr)
    , m_isModified(false)
    , m_currentZoom(100.0)
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

    if (m_document) {
    }

    m_document = document;
    m_scene->setDocument(document);
    m_styleManager->setDocument(document);
}

void MainWindow::setupUi()
{
    resize(800, 600);

    QWidget *centralContainer = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralContainer);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    centralContainer->setLayout(mainLayout);
    setCentralWidget(centralContainer);

    m_ribbonBar = new RibbonBar(this);
    m_ribbonBar->setFixedHeight(Constants::RIBBON_BAR_HEIGHT);
    mainLayout->addWidget(m_ribbonBar);

    m_view = new DocumentView(this);
    m_scene = new DocumentScene(this);
    m_view->setScene(m_scene);
    
    QWidget *viewContainer = new QWidget(this);
    QVBoxLayout *viewLayout = new QVBoxLayout(viewContainer);
    viewLayout->setContentsMargins(0, Constants::PAGE_TOP_SPACING, 0, 0);
    viewLayout->setSpacing(0);
    viewLayout->addWidget(m_view);
    viewContainer->setLayout(viewLayout);
    
    mainLayout->addWidget(viewContainer);

    m_document = new Document(this);
    m_cursor = new Cursor(m_document, this);
    m_selection = new Selection(m_document, this);
    m_editEventHandler = new EditEventHandler(m_document, m_cursor, m_selection, this);
    m_formatController = new FormatController(m_document, m_selection, this);
    m_styleManager = new StyleManager(this);

    connect(m_view, &DocumentView::keyPressed,
            m_editEventHandler, &EditEventHandler::handleKeyPress);
    connect(m_view, &DocumentView::mousePressed,
            m_editEventHandler, &EditEventHandler::handleMousePress);
    connect(m_view, &DocumentView::mouseMoved,
            m_editEventHandler, &EditEventHandler::handleMouseMove);
    connect(m_view, &DocumentView::mouseReleased,
            m_editEventHandler, &EditEventHandler::handleMouseRelease);

    connect(m_document, &Document::documentChanged,
            this, &MainWindow::updateWindowTitle);

    connect(m_cursor, &Cursor::positionChanged,
            this, &MainWindow::updateUI);

    m_currentZoom = 100.0;
    
    connect(m_view, &DocumentView::mousePositionChanged,
            this, [this](const QPointF &scenePos, const QPoint &viewPos) {
                updateStatusBar(scenePos, viewPos);
            });
    
    connect(m_view, &DocumentView::zoomChanged,
            this, [this](qreal zoom) {
                m_currentZoom = zoom;
                if (m_lastScenePos.isNull()) {
                    QPoint center = m_view->viewport()->rect().center();
                    QPointF scenePos = m_view->mapToScene(center);
                    updateStatusBar(scenePos, center);
                } else {
                    updateStatusBar(m_lastScenePos, m_lastViewPos);
                }
            });

    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::createActions()
{
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

    QAction *fontAct = new QAction(tr("&Font..."), this);
    connect(fontAct, &QAction::triggered, this, [this]() {
        qDebug() << "Font dialog placeholder";
    });

    QAction *paragraphAct = new QAction(tr("&Paragraph..."), this);
    connect(paragraphAct, &QAction::triggered, this, [this]() {
        qDebug() << "Paragraph dialog placeholder";
    });

    QAction *imageAct = new QAction(tr("&Image..."), this);
    connect(imageAct, &QAction::triggered, this, [this]() {
        qDebug() << "Insert image dialog placeholder";
    });

    QAction *zoomInAct = new QAction(tr("Zoom &In"), this);
    zoomInAct->setShortcut(QKeySequence::ZoomIn);
    connect(zoomInAct, &QAction::triggered, this, &MainWindow::zoomIn);

    QAction *zoomOutAct = new QAction(tr("Zoom &Out"), this);
    zoomOutAct->setShortcut(QKeySequence::ZoomOut);
    connect(zoomOutAct, &QAction::triggered, this, &MainWindow::zoomOut);

    QAction *zoomToFitAct = new QAction(tr("Zoom to &Fit"), this);
    connect(zoomToFitAct, &QAction::triggered, this, &MainWindow::zoomToFit);

    QAction *pageSetupAct = new QAction(tr("Page &Setup..."), this);
    connect(pageSetupAct, &QAction::triggered, this, &MainWindow::pageSetup);

    QAction *aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, &QAction::triggered, this, &MainWindow::about);

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(pageSetupAct);
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
        
        Section *section = new Section(m_document);
        m_document->addSection(section);
        
        ParagraphBlock *para1 = new ParagraphBlock(section);
        para1->setText("欢迎使用 QtWordEditor!");
        section->addBlock(para1);
        
        ParagraphBlock *para2 = new ParagraphBlock(section);
        para2->setText("这是一个功能丰富的文字处理软件，使用 Qt 框架开发。");
        section->addBlock(para2);
        
        ParagraphBlock *para3 = new ParagraphBlock(section);
        para3->setText("主要功能包括：");
        section->addBlock(para3);
        
        ParagraphBlock *para4 = new ParagraphBlock(section);
        para4->setText("- 富文本编辑");
        section->addBlock(para4);
        
        ParagraphBlock *para5 = new ParagraphBlock(section);
        para5->setText("- 段落格式化");
        section->addBlock(para5);
        
        ParagraphBlock *para6 = new ParagraphBlock(section);
        para6->setText("- 多页文档支持");
        section->addBlock(para6);
        
        qreal pageWidth = Constants::PAGE_WIDTH;
        qreal pageHeight = Constants::PAGE_HEIGHT;
        qreal margin = Constants::PAGE_MARGIN;
        
        m_scene->clearPages();
        
        for (int pageNum = 0; pageNum < 3; ++pageNum) {
            PageBuilder builder(pageWidth, pageHeight, margin);
            
            for (int i = 0; i < section->blockCount(); ++i) {
                Block *block = section->block(i);
                block->setHeight(30.0);
                builder.tryAddBlock(block);
            }
            
            Page *page = builder.finishPage();
            page->setPageNumber(pageNum + 1);
            section->addPage(page);
            m_scene->addPage(page);
        }
        
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

    newDocument();
    m_currentFile = fileName;
    m_isModified = false;
    statusBar()->showMessage(tr("Loaded %1").arg(fileName));
}

bool MainWindow::saveDocument()
{
    if (m_currentFile.isEmpty())
        return saveAsDocument();
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
}

void MainWindow::copy()
{
}

void MainWindow::paste()
{
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
    if (m_ribbonBar) {
        m_ribbonBar->updateFromSelection();
    }
}

void MainWindow::updateStatusBar(const QPointF &scenePos, const QPoint &viewPos)
{
    m_lastScenePos = scenePos;
    m_lastViewPos = viewPos;
    
    Page *page = m_scene->pageAt(scenePos);
    
    if (page) {
        qreal pageSpacing = 30.0;
        qreal pageHeight = Constants::PAGE_HEIGHT;
        int pageIndex = page->pageNumber() - 1;
        qreal yOffset = pageIndex * (pageHeight + pageSpacing);
        
        qreal relativeX = scenePos.x();
        qreal relativeY = scenePos.y() - yOffset;
        
        QString statusText = QString("缩放: %1%  |  页码: %2  |  场景坐标: (%3, %4)  |  相对坐标: (%5, %6)  |  视图坐标: (%7, %8)")
            .arg(m_currentZoom, 0, 'f', 0)
            .arg(page->pageNumber())
            .arg(scenePos.x(), 0, 'f', 2)
            .arg(scenePos.y(), 0, 'f', 2)
            .arg(relativeX, 0, 'f', 2)
            .arg(relativeY, 0, 'f', 2)
            .arg(viewPos.x())
            .arg(viewPos.y());
        statusBar()->showMessage(statusText);
    } else {
        QString statusText = QString("缩放: %1%  |  场景坐标: (%2, %3)  |  视图坐标: (%4, %5)")
            .arg(m_currentZoom, 0, 'f', 0)
            .arg(scenePos.x(), 0, 'f', 2)
            .arg(scenePos.y(), 0, 'f', 2)
            .arg(viewPos.x())
            .arg(viewPos.y());
        statusBar()->showMessage(statusText);
    }
}

void MainWindow::pageSetup()
{
    PageSetup newSetup = PageSetupDialog::getPageSetup(m_pageSetup, this);
    
    bool changed = false;
    if (!qFuzzyCompare(newSetup.pageWidth, m_pageSetup.pageWidth)) changed = true;
    if (!qFuzzyCompare(newSetup.pageHeight, m_pageSetup.pageHeight)) changed = true;
    if (!qFuzzyCompare(newSetup.marginLeft, m_pageSetup.marginLeft)) changed = true;
    if (!qFuzzyCompare(newSetup.marginRight, m_pageSetup.marginRight)) changed = true;
    if (!qFuzzyCompare(newSetup.marginTop, m_pageSetup.marginTop)) changed = true;
    if (!qFuzzyCompare(newSetup.marginBottom, m_pageSetup.marginBottom)) changed = true;
    if (newSetup.portrait != m_pageSetup.portrait) changed = true;
    
    if (changed) {
        m_pageSetup = newSetup;
        m_isModified = true;
        updateWindowTitle();
        qDebug() << "Page setup changed:";
        qDebug() << "  Size:" << newSetup.pageWidth << "x" << newSetup.pageHeight << "mm";
        qDebug() << "  Margins:" << newSetup.marginLeft << newSetup.marginRight << newSetup.marginTop << newSetup.marginBottom << "mm";
        qDebug() << "  Portrait:" << newSetup.portrait;
    }
}

} // namespace QtWordEditor
