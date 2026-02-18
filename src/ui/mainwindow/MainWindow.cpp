#include "ui/mainwindow/MainWindow.h"
#include "app/Application.h"
#include "core/document/Document.h"
#include "core/document/Section.h"
#include "core/document/ParagraphBlock.h"
#include "core/document/Span.h"
#include "core/document/ImageBlock.h"
#include "core/document/TableBlock.h"
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
#include "ui/widgets/DebugConsole.h"
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QDockWidget>
#include <QScreen>
#include <QGuiApplication>
#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QCloseEvent>
#include <QVBoxLayout>
#include <QWidget>
#include <QDebug>
#include <QFontMetrics>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QTextOption>
#include <QAbstractTextDocumentLayout>

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
    qDebug() << "MainWindow::setDocument called, document =" << document;
    qDebug() << "  m_document before =" << m_document;
    
    if (m_document) {
    }

    m_document = document;
    m_scene->setDocument(document);
    m_styleManager->setDocument(document);
    m_scene->rebuildFromDocument();
}

void MainWindow::setupUi()
{
    resize(1200, 700);
    
    // 窗口居中显示
    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
        QRect screenGeometry = screen->availableGeometry();
        int x = (screenGeometry.width() - width()) / 2;
        int y = (screenGeometry.height() - height()) / 2;
        move(x, y);
    }

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
            this, [this](const QPointF &scenePos) {
                CursorPosition pos = m_scene->cursorPositionAt(scenePos);
                m_cursor->setPosition(pos);
            });
    connect(m_view, &DocumentView::mouseMoved,
            m_editEventHandler, &EditEventHandler::handleMouseMove);
    connect(m_view, &DocumentView::mouseReleased,
            m_editEventHandler, &EditEventHandler::handleMouseRelease);
    connect(m_view, &DocumentView::inputMethodReceived,
            m_editEventHandler, &EditEventHandler::handleInputMethod);
    
    // 设置光标到 DocumentView
    m_view->setCursor(m_cursor);

    connect(m_document, &Document::documentChanged,
            this, &MainWindow::updateWindowTitle);

    connect(m_cursor, &Cursor::positionChanged,
            this, &MainWindow::updateCursorPosition);

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

    m_view->setFocus();
    m_view->activateWindow();
    
    // 创建自定义状态栏
    setupCustomStatusBar();
    
    // 创建调试控制台
    setupDebugConsole();
    
    // 先创建新文档，这会调用 setDocument()
    newDocument();
    
    m_cursor->setPosition(0, 0);
    m_currentCursorPos = m_cursor->position();
    QPointF initialPos = calculateCursorVisualPosition(m_currentCursorPos);
    m_scene->updateCursor(initialPos, 20.0);
    m_view->setCursorVisualPosition(initialPos);
    
    // 初始化状态栏显示初始位置
    if (m_lastScenePos.isNull()) {
        QPoint center = m_view->viewport()->rect().center();
        QPointF scenePos = m_view->mapToScene(center);
        updateStatusBar(scenePos, center);
    } else {
        updateStatusBar(m_lastScenePos, m_lastViewPos);
    }
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

    QAction *chineseAct = new QAction(tr("&Chinese"), this);
    connect(chineseAct, &QAction::triggered, this, &MainWindow::switchToChinese);

    QAction *englishAct = new QAction(tr("&English"), this);
    connect(englishAct, &QAction::triggered, this, &MainWindow::switchToEnglish);

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

    QMenu *languageMenu = menuBar()->addMenu(tr("&Language"));
    languageMenu->addAction(chineseAct);
    languageMenu->addAction(englishAct);

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
        while (m_document->sectionCount() > 0) {
            m_document->removeSection(0);
        }
        
        Section *section = new Section(m_document);
        m_document->addSection(section);
        
        ParagraphBlock *para1 = new ParagraphBlock(section);
        para1->setText("");
        section->addBlock(para1);
        
        qreal pageWidth = Constants::PAGE_WIDTH;
        qreal pageHeight = Constants::PAGE_HEIGHT;
        qreal margin = Constants::PAGE_MARGIN;
        
        m_scene->clearPages();
        
        PageBuilder builder(pageWidth, pageHeight, margin);
        for (int i = 0; i < section->blockCount(); ++i) {
            Block *block = section->block(i);
            block->setHeight(30.0);
            builder.tryAddBlock(block);
            
            ParagraphBlock *paraBlock = qobject_cast<ParagraphBlock*>(block);
            if (paraBlock) {
                // 连接 textChanged 信号，当文本变化时重建场景
                connect(paraBlock, &ParagraphBlock::textChanged, this, [this]() {
                    qDebug() << "ParagraphBlock textChanged, rebuilding scene";
                    m_scene->rebuildFromDocument();
                });
                qDebug() << "  ParagraphBlock created, text =" << paraBlock->text();
            }
        }
        
        Page *page = builder.finishPage();
        page->setPageNumber(1);
        section->addPage(page);
        
        // 现在调用 setDocument，这会调用 m_scene->setDocument 和 rebuildFromDocument
        setDocument(m_document);
        
        // 重置光标位置到 (0, 0)
        m_cursor->setPosition(0, 0);
        
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
    
    // ========== 收集文档信息 ==========
    QString blockType = "N/A";
    QString spanInfo = "N/A";
    int spanIndex = -1;
    int spanOffset = -1;
    QString characterInfo = "N/A";
    QString paragraphLength = "N/A";
    QString spanCount = "N/A";
    
    if (m_document && m_document->sectionCount() > 0) {
        Section *section = m_document->section(0);
        if (section && m_currentCursorPos.blockIndex >= 0 && m_currentCursorPos.blockIndex < section->blockCount()) {
            Block *block = section->block(m_currentCursorPos.blockIndex);
            if (block) {
                // 获取块类型
                if (qobject_cast<ParagraphBlock*>(block)) {
                    blockType = "段落块";
                    
                    ParagraphBlock *paraBlock = qobject_cast<ParagraphBlock*>(block);
                    paragraphLength = QString::number(paraBlock->length());
                    spanCount = QString::number(paraBlock->spanCount());
                    
                    // 查找 Span 信息
                    int posInSpan = 0;
                    spanIndex = paraBlock->findSpanIndex(m_currentCursorPos.offset, &posInSpan);
                    spanOffset = posInSpan;
                    
                    if (spanIndex >= 0 && spanIndex < paraBlock->spanCount()) {
                        Span span = paraBlock->span(spanIndex);
                        spanInfo = QString("索引:%1, 长度:%2, 偏移:%3")
                            .arg(spanIndex)
                            .arg(span.length())
                            .arg(posInSpan);
                        
                        // 获取字符信息
                        if (m_currentCursorPos.offset >= 0 && m_currentCursorPos.offset < paraBlock->text().length()) {
                            QChar ch = paraBlock->text().at(m_currentCursorPos.offset);
                            characterInfo = QString("'%1' (0x%2)")
                                .arg(ch.isPrint() ? ch : '?')
                                .arg(ch.unicode(), 4, 16, QChar('0'));
                        }
                    }
                } else if (qobject_cast<ImageBlock*>(block)) {
                    blockType = "图片块";
                } else if (qobject_cast<TableBlock*>(block)) {
                    blockType = "表格块";
                }
            }
        }
    }
    
    // ========== 第一行：光标和位置信息 ==========
    QString line1 = QString("光标: 块%1, 偏移%2  |  缩放: %3%  |  块类型: %4")
        .arg(m_currentCursorPos.blockIndex)
        .arg(m_currentCursorPos.offset)
        .arg(m_currentZoom, 0, 'f', 0)
        .arg(blockType);
    
    if (page) {
        qreal pageSpacing = 30.0;
        qreal pageHeight = Constants::PAGE_HEIGHT;
        int pageIndex = page->pageNumber() - 1;
        qreal yOffset = pageIndex * (pageHeight + pageSpacing);
        
        qreal relativeX = scenePos.x();
        qreal relativeY = scenePos.y() - yOffset;
        
        line1 += QString("  |  页码: %1  |  场景: (%2, %3)  |  相对: (%4, %5)  |  视图: (%6, %7)")
            .arg(page->pageNumber())
            .arg(scenePos.x(), 0, 'f', 1)
            .arg(scenePos.y(), 0, 'f', 1)
            .arg(relativeX, 0, 'f', 1)
            .arg(relativeY, 0, 'f', 1)
            .arg(viewPos.x())
            .arg(viewPos.y());
    } else {
        line1 += QString("  |  场景: (%1, %2)  |  视图: (%3, %4)")
            .arg(scenePos.x(), 0, 'f', 1)
            .arg(scenePos.y(), 0, 'f', 1)
            .arg(viewPos.x())
            .arg(viewPos.y());
    }
    
    // ========== 第二行：Span 和详细信息 ==========
    QString line2 = "";
    if (spanIndex >= 0) {
        line2 = QString("Span: %1  |  段落长度: %2  |  Span数量: %3")
            .arg(spanInfo)
            .arg(paragraphLength)
            .arg(spanCount);
        
        if (characterInfo != "N/A") {
            line2 += QString("  |  字符: %1").arg(characterInfo);
        }
    } else {
        line2 = QString("段落长度: %1  |  Span数量: %2")
            .arg(paragraphLength)
            .arg(spanCount);
    }
    
    // 更新状态栏标签
    if (m_statusLine1Label) {
        m_statusLine1Label->setText(line1);
    }
    if (m_statusLine2Label) {
        m_statusLine2Label->setText(line2);
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

void MainWindow::switchToChinese()
{
    Application *app = Application::instance();
    if (app) {
        app->switchLanguage("zh_CN");
        retranslateUi();
    }
}

void MainWindow::switchToEnglish()
{
    Application *app = Application::instance();
    if (app) {
        app->switchLanguage("en_US");
        retranslateUi();
    }
}

void MainWindow::retranslateUi()
{
    menuBar()->clear();
    createActions();
    updateWindowTitle();
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::updateCursorPosition(const CursorPosition &pos)
{
    m_currentCursorPos = pos;
    QPointF visualPos = calculateCursorVisualPosition(pos);
    qreal cursorHeight = 20.0;
    m_scene->updateCursor(visualPos, cursorHeight);
    m_view->setCursorVisualPosition(visualPos);
    
    // 同时更新状态栏，显示光标位置
    updateStatusBar(m_lastScenePos, m_lastViewPos);
}

QPointF MainWindow::calculateCursorVisualPosition(const CursorPosition &pos)
{
    qDebug() << "calculateCursorVisualPosition called, pos:" << pos.blockIndex << "," << pos.offset;
    
    if (m_scene) {
        QPointF result = m_scene->calculateCursorVisualPosition(pos);
        qDebug() << "  Returning position from scene:" << result;
        return result;
    }
    
    qDebug() << "  m_scene is null, returning (0,0)";
    return QPointF(0, 0);
}

void MainWindow::setupDebugConsole()
{
    m_debugConsoleDock = new QDockWidget(tr("调试控制台"), this);
    m_debugConsoleDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    
    QWidget *dockContent = new QWidget(m_debugConsoleDock);
    QVBoxLayout *dockLayout = new QVBoxLayout(dockContent);
    dockLayout->setContentsMargins(4, 4, 4, 4);
    dockLayout->setSpacing(4);
    
    m_debugConsoleTextEdit = new QPlainTextEdit(dockContent);
    m_debugConsoleTextEdit->setReadOnly(true);
    m_debugConsoleTextEdit->setMaximumBlockCount(1000);
    QFont font("Consolas", 9);
    m_debugConsoleTextEdit->setFont(font);
    dockLayout->addWidget(m_debugConsoleTextEdit);
    
    QPushButton *clearButton = new QPushButton(tr("清空"), dockContent);
    connect(clearButton, &QPushButton::clicked, this, [this]() {
        m_debugConsoleTextEdit->clear();
        DebugConsole::log(tr("调试控制台已清空"));
    });
    dockLayout->addWidget(clearButton);
    
    dockContent->setLayout(dockLayout);
    m_debugConsoleDock->setWidget(dockContent);
    
    addDockWidget(Qt::RightDockWidgetArea, m_debugConsoleDock);
    
    // 连接 DebugConsole 的信号到我们的槽
    connect(DebugConsole::instance(), &DebugConsole::logMessage,
            this, [this](const QString &message) {
        m_debugConsoleTextEdit->appendPlainText(message);
        QTextCursor cursor = m_debugConsoleTextEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        m_debugConsoleTextEdit->setTextCursor(cursor);
    });
    
    // 安装消息处理器，捕获所有 qDebug() 输出
    DebugConsole::installMessageHandler();
    
    // 发送欢迎消息
    DebugConsole::log(tr("调试控制台初始化成功！"));
}

void MainWindow::setupCustomStatusBar()
{
    // 创建自定义状态栏容器
    m_statusBarWidget = new QWidget(this);
    QVBoxLayout *statusLayout = new QVBoxLayout(m_statusBarWidget);
    statusLayout->setContentsMargins(5, 2, 5, 2);
    statusLayout->setSpacing(2);
    
    // 创建第一行标签
    m_statusLine1Label = new QLabel(tr("准备就绪"), this);
    m_statusLine1Label->setFrameStyle(QFrame::NoFrame);
    m_statusLine1Label->setStyleSheet("color: #333; font-size: 10pt;");
    
    // 创建第二行标签
    m_statusLine2Label = new QLabel("", this);
    m_statusLine2Label->setFrameStyle(QFrame::NoFrame);
    m_statusLine2Label->setStyleSheet("color: #666; font-size: 9pt;");
    
    statusLayout->addWidget(m_statusLine1Label);
    statusLayout->addWidget(m_statusLine2Label);
    m_statusBarWidget->setLayout(statusLayout);
    
    // 将自定义部件添加到状态栏
    statusBar()->addWidget(m_statusBarWidget, 1);
}

} // namespace QtWordEditor
