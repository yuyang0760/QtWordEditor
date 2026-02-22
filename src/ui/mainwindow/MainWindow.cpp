#include "ui/mainwindow/MainWindow.h"
#include "app/Application.h"
#include "core/document/Document.h"
#include "core/document/Section.h"
#include "core/document/ParagraphBlock.h"
#include "core/document/ImageBlock.h"
#include "core/document/CharacterStyle.h"
#include "core/document/TableBlock.h"
#include "core/document/Page.h"
#include "core/document/math/NumberMathSpan.h"
#include "core/document/math/FractionMathSpan.h"
#include "core/document/math/RowContainerMathSpan.h"
#include "core/layout/PageBuilder.h"
#include "core/utils/Constants.h"
// 移除 Logger 头文件，使用 Qt 内置日志函数
#include "graphics/scene/DocumentScene.h"
#include "graphics/view/DocumentView.h"
#include "editcontrol/cursor/Cursor.h"
#include "editcontrol/selection/Selection.h"
#include "editcontrol/handlers/EditEventHandler.h"
#include "editcontrol/formatting/FormatController.h"
#include "core/styles/StyleManager.h"
#include "ui/ribbon/RibbonBar.h"
#include "ui/dialogs/PageSetupDialog.h"
#include "ui/dialogs/StyleManagerDialog.h"
#include "ui/dialogs/ParagraphDialog.h"
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
  //  QDebug() << "MainWindow::setDocument - 设置文档，文档指针:" << document;
  //  QDebug() << "  原文档指针:" << m_document;
    
    if (m_document) {
    }

    m_document = document;
    // m_scene->setDocument(document);  // 这里面会调用 rebuildFromDocument()，避免重复调用
    m_styleManager->setDocument(document);
    m_scene->rebuildFromDocument();
}

void MainWindow::setupUi()
{
    resize(1300, 700);
    
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

    m_document = new Document(this);
    m_cursor = new Cursor(m_document, this);
    m_selection = new Selection(m_document, this);
    m_styleManager = new StyleManager(this);
    m_formatController = new FormatController(m_document, m_cursor, m_selection, m_styleManager, this);
    m_editEventHandler = new EditEventHandler(m_document, m_cursor, m_selection, m_formatController, this);

    m_ribbonBar = new RibbonBar(m_styleManager, this);
    m_ribbonBar->setFixedHeight(Constants::RIBBON_BAR_HEIGHT);
    mainLayout->addWidget(m_ribbonBar);

    m_view = new DocumentView(this);
    m_scene = new DocumentScene(this);
    m_view->setScene(m_scene);
    m_scene->setDocument(m_document);
    
    QWidget *viewContainer = new QWidget(this);
    QVBoxLayout *viewLayout = new QVBoxLayout(viewContainer);
    viewLayout->setContentsMargins(0, Constants::PAGE_TOP_SPACING, 0, 0);
    viewLayout->setSpacing(0);
    viewLayout->addWidget(m_view);
    viewContainer->setLayout(viewLayout);
    
    mainLayout->addWidget(viewContainer);

    connect(m_styleManager, &StyleManager::characterStyleChanged,
            this, &MainWindow::onStyleChanged);

    connect(m_styleManager, &StyleManager::paragraphStyleChanged,
            this, &MainWindow::onStyleChanged);

    connect(m_styleManager, &StyleManager::stylesChanged,
            this, &MainWindow::onStyleChanged);

    // 连接 RibbonBar 样式按钮信号
    connect(m_ribbonBar, &RibbonBar::fontChanged,
            this, [this](const QFont &font) {
        qDebug() << "字体变化（来自功能区）:" << font.family();
        // 只设置字体族，不改变字号！
        CharacterStyle style;
        style.clearAllProperties();
        style.setFontFamily(font.family());
        if (m_selection->isEmpty()) {
            m_formatController->setCurrentInputStyle(style);
        } else {
            // 使用单独的 setFontFamily 方法（我们需要添加）
            m_formatController->setFontFamily(font.family());
        }
        updateStyleState();
    });

    connect(m_ribbonBar, &RibbonBar::fontSizeChanged,
            this, [this](int size) {
      //  QDebug() << "字体大小变化（来自功能区）:" << size;
        CharacterStyle style;
        style.setFontSize(size);
        if (m_selection->isEmpty()) {
            m_formatController->setCurrentInputStyle(style);
        } else {
            m_formatController->setFontSize(size);
        }
        updateStyleState();
    });

    connect(m_ribbonBar, &RibbonBar::boldChanged,
            this, [this]() {
        toggleStyleAttribute(
            [](const CharacterStyle& s) { return s.bold(); },
            [](CharacterStyle& s, bool v) { s.setBold(v); },
            [this](bool v) { m_formatController->setBold(v); },
            "加粗"
        );
    });

    connect(m_ribbonBar, &RibbonBar::italicChanged,
            this, [this]() {
        toggleStyleAttribute(
            [](const CharacterStyle& s) { return s.italic(); },
            [](CharacterStyle& s, bool v) { s.setItalic(v); },
            [this](bool v) { m_formatController->setItalic(v); },
            "斜体"
        );
    });

    connect(m_ribbonBar, &RibbonBar::underlineChanged,
            this, [this]() {
        toggleStyleAttribute(
            [](const CharacterStyle& s) { return s.underline(); },
            [](CharacterStyle& s, bool v) { s.setUnderline(v); },
            [this](bool v) { m_formatController->setUnderline(v); },
            "下划线"
        );
    });

    // 连接 RibbonBar 样式选择信号
    connect(m_ribbonBar, &RibbonBar::characterStyleChanged,
            this, [this](const QString &styleName) {
      //  QDebug() << "选择字符样式（来自功能区）:" << styleName;
        if (m_selection->isEmpty() && m_styleManager && m_styleManager->hasCharacterStyle(styleName)) {
            CharacterStyle style = m_styleManager->getResolvedCharacterStyle(styleName);
            m_formatController->setCurrentInputStyle(style);
        } else {
            m_formatController->applyNamedCharacterStyle(styleName);
        }
        updateStyleState();
    });

    connect(m_ribbonBar, &RibbonBar::paragraphStyleChanged,
            this, [this](const QString &styleName) {
      //  QDebug() << "选择段落样式（来自功能区）:" << styleName;
        m_formatController->applyNamedParagraphStyle(styleName);
    });

    connect(m_ribbonBar, &RibbonBar::openStyleManagerRequested,
            this, [this]() {
      //  QDebug() << "请求打开样式管理器";
        StyleManagerDialog dialog(m_styleManager, this);
        dialog.exec();
        // 对话框关闭后刷新样式列表
        if (m_ribbonBar) {
            m_ribbonBar->refreshStyleLists();
        }
    });
    
    // ========== 连接公式工具栏信号 ==========
    connect(m_ribbonBar, &RibbonBar::insertFormulaRequested,
            this, [this]() {
        qDebug() << "插入公式";
        // 暂时添加简单的测试公式
        insertTestFormula();
    });
    
    connect(m_ribbonBar, &RibbonBar::insertFractionRequested,
            this, [this]() {
        qDebug() << "插入分数";
        // 在光标处插入分数
        insertFractionAtCursor();
    });
    
    connect(m_ribbonBar, &RibbonBar::insertNumberRequested,
            this, [this]() {
        qDebug() << "插入公式文本";
        // 在光标处插入公式文本
        insertNumberAtCursor();
    });
    
    connect(m_ribbonBar, &RibbonBar::insertRadicalRequested,
            this, [this]() {
        qDebug() << "插入根号";
    });
    
    connect(m_ribbonBar, &RibbonBar::insertSubscriptRequested,
            this, [this]() {
        qDebug() << "插入下标";
    });
    
    connect(m_ribbonBar, &RibbonBar::insertSuperscriptRequested,
            this, [this]() {
        qDebug() << "插入上标";
    });

    // 设置场景到 EditEventHandler
    m_editEventHandler->setScene(m_scene);
    
    connect(m_view, &DocumentView::keyPressed,
            m_editEventHandler, &EditEventHandler::handleKeyPress);
    
    // 连接鼠标事件
    connect(m_view, &DocumentView::mousePressed,
            m_editEventHandler, &EditEventHandler::handleMousePress);
    connect(m_view, &DocumentView::mouseMoved,
            m_editEventHandler, &EditEventHandler::handleMouseMove);
    connect(m_view, &DocumentView::mouseReleased,
            m_editEventHandler, &EditEventHandler::handleMouseRelease);
    connect(m_view, &DocumentView::inputMethodReceived,
            m_editEventHandler, &EditEventHandler::handleInputMethod);
    
    // 连接右键菜单事件
    connect(m_view, &DocumentView::contextMenuParagraphRequested,
            this, &MainWindow::paragraphSettings);
    
    // 连接选择更新信号
    connect(m_editEventHandler, &EditEventHandler::selectionNeedsUpdate,
            this, [this]() {
                if (m_selection && m_scene) {
                    if (m_selection->isEmpty()) {
                        m_scene->clearSelection();
                    } else {
                        QList<QRectF> rects = m_scene->calculateSelectionRects(m_selection->range());
                        m_scene->updateSelection(rects);
                    }
                    // 始终显示光标
                    m_scene->setCursorVisible(true);
                }
            });
    
    // 设置光标到 DocumentView
    m_view->setCursor(m_cursor);

    connect(m_document, &Document::documentChanged,
            this, &MainWindow::updateWindowTitle);

    connect(m_cursor, &Cursor::positionChanged,
            m_formatController, &FormatController::onCursorMoved);
    connect(m_cursor, &Cursor::positionChanged,
            this, &MainWindow::updateCursorPosition);
    
    // 连接光标位置变化信号到样式状态更新（无选区时）
    connect(m_cursor, &Cursor::positionChanged,
            this, [this]() {
                // 只有在无选区时，光标移动才更新样式
                if (m_selection && m_selection->isEmpty()) {
                    updateStyleState();
                }
            });
    
    // 连接选区变化信号（只更新光标可见性，不更新样式）
    connect(m_selection, &Selection::selectionChanged,
            this, [this]() {
                if (m_selection && m_scene) {
                    // 始终显示光标
                    m_scene->setCursorVisible(true);
                }
                // 不在这里更新样式，只在鼠标松开时更新
            });
    
    // 连接选择完成信号（鼠标松开时）到样式状态更新
    connect(m_editEventHandler, &EditEventHandler::selectionFinished,
            this, [this]() {
        qDebug() << "MainWindow: 收到 selectionFinished 信号，更新样式状态";
        updateStyleState();
    });

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
      //  QDebug() << "字体对话框（占位）";
    });

    QAction *paragraphAct = new QAction(tr("&Paragraph..."), this);
    connect(paragraphAct, &QAction::triggered, this, &MainWindow::paragraphSettings);

    QAction *imageAct = new QAction(tr("&Image..."), this);
    connect(imageAct, &QAction::triggered, this, [this]() {
      //  QDebug() << "插入图片对话框（占位）";
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
        para1->setText("这是一段很长很长的测试文字。我们用来测试自动换行功能。这段文字应该会超过页面的宽度，这样我们可以看到自动换行是否正常工作。欢迎使用 QtWordEditor 文字编辑器！这是一个功能强大的文字编辑器，支持各种文字格式和布局。");
        section->addBlock(para1);
        
        ParagraphBlock *para2 = new ParagraphBlock(section);
        para2->setText("这是第二段测试文字。您可以在这里进行各种文字编辑操作，包括字体样式修改、段落对齐等功能。让我们继续输入一些文字，看看第二段是否也能正常换行！");
        section->addBlock(para2);
        
        // ========== 测试段落：包含 MathSpan 的段落 ==========
        ParagraphBlock *para3 = new ParagraphBlock(section);
        // 先清空所有 InlineSpan
        para3->clearInlineSpans();
        
        // 添加文字："公式测试: "
        para3->insert(0, "公式测试: ", CharacterStyle());
        
        // 创建分数公式：123/456
        auto numerator = new NumberMathSpan("123");
        auto denominator = new NumberMathSpan("456");
        auto fraction = new FractionMathSpan(numerator, denominator);
        para3->addInlineSpan(fraction);
        
        // 添加文字：" 继续测试 "
        para3->insert(para3->length(), " 继续测试 ", CharacterStyle());
        
        // 创建另一个公式：789
        auto number = new NumberMathSpan("789");
        para3->addInlineSpan(number);
        
        // 添加文字：" 测试完毕！"
        para3->insert(para3->length(), " 测试完毕！", CharacterStyle());
        
        section->addBlock(para3);
        
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
                // 连接 textChanged 信号，只更新当前修改的块而不是全部
                connect(paraBlock, &ParagraphBlock::textChanged, this, [this, block]() {
                    m_scene->updateSingleTextItem(block);
                });
            }
        }
        
        Page *page = builder.finishPage();
        page->setPageNumber(1);
        section->addPage(page);
        
        // 直接调用 rebuildFromDocument，避免重复调用
        m_scene->rebuildFromDocument();
        
        // 重置光标位置到 (0, 0)
        m_cursor->setPosition(0, 0);
        
        m_currentFile.clear();
        m_isModified = false;
    }
}

void MainWindow::insertTestFormula()
{
    if (!m_document || m_document->sectionCount() == 0) {
        return;
    }
    
    Section *section = m_document->section(0);
    if (!section) {
        return;
    }
    
    ParagraphBlock *para = new ParagraphBlock(section);
    para->clearInlineSpans();
    
    para->insert(0, "插入的公式: ", CharacterStyle());
    
    auto number = new NumberMathSpan("x");
    para->addInlineSpan(number);
    
    para->insert(para->length(), " = ", CharacterStyle());
    
    auto numerator = new NumberMathSpan("a+b");
    auto denominator = new NumberMathSpan("c");
    auto fraction = new FractionMathSpan(numerator, denominator);
    para->addInlineSpan(fraction);
    
    para->insert(para->length(), " + 1", CharacterStyle());
    
    section->addBlock(para);
    
    // ========== 关键修复：更新页面 ==========
    if (section->pageCount() > 0) {
        Page *page = section->page(0);
        para->setHeight(30.0);
        
        // 连接 textChanged 信号
        connect(para, &ParagraphBlock::textChanged, this, [this, para]() {
            m_scene->updateSingleTextItem(para);
        });
        
        // 将新块添加到页面
        page->addBlock(para);
    }
    
    // 重新构建文档
    m_scene->rebuildFromDocument();
}

void MainWindow::insertTestFraction()
{
    if (!m_document || m_document->sectionCount() == 0) {
        return;
    }
    
    Section *section = m_document->section(0);
    if (!section) {
        return;
    }
    
    ParagraphBlock *para = new ParagraphBlock(section);
    para->clearInlineSpans();
    
    para->insert(0, "插入的分数: ", CharacterStyle());
    
    auto numerator = new NumberMathSpan("100");
    auto denominator = new NumberMathSpan("200");
    auto fraction = new FractionMathSpan(numerator, denominator);
    para->addInlineSpan(fraction);
    
    para->insert(para->length(), " 测试完成", CharacterStyle());
    
    section->addBlock(para);
    
    // ========== 关键修复：更新页面 ==========
    if (section->pageCount() > 0) {
        Page *page = section->page(0);
        para->setHeight(30.0);
        
        // 连接 textChanged 信号
        connect(para, &ParagraphBlock::textChanged, this, [this, para]() {
            m_scene->updateSingleTextItem(para);
        });
        
        // 将新块添加到页面
        page->addBlock(para);
    }
    
    // 重新构建文档
    m_scene->rebuildFromDocument();
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
                    spanCount = QString::number(paraBlock->inlineSpanCount());
                    
                    // 查找 InlineSpan 信息
                    int posInSpan = 0;
                    spanIndex = paraBlock->findInlineSpanIndex(m_currentCursorPos.offset, &posInSpan);
                    spanOffset = posInSpan;
                    
                    if (spanIndex >= 0 && spanIndex < paraBlock->inlineSpanCount()) {
                        InlineSpan *inlineSpan = paraBlock->inlineSpan(spanIndex);
                        int spanLen = inlineSpan->length();
                        spanInfo = QString("索引:%1, 长度:%2, 偏移:%3")
                            .arg(spanIndex)
                            .arg(spanLen)
                            .arg(posInSpan);
                        
                        // 获取字符信息
                        QChar ch = paraBlock->characterAt(m_currentCursorPos.offset);
                        if (!ch.isNull()) {
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
      //  QDebug() << "页面设置变化:";
      //  QDebug() << "  尺寸:" << newSetup.pageWidth << "x" << newSetup.pageHeight << "mm";
      //  QDebug() << "  边距:" << newSetup.marginLeft << newSetup.marginRight << newSetup.marginTop << newSetup.marginBottom << "mm";
      //  QDebug() << "  纵向:" << newSetup.portrait;
    }
}

void MainWindow::paragraphSettings()
{
    if (!m_formatController) {
        return;
    }
    
    // 获取当前段落样式
    ParagraphStyle initialStyle = m_formatController->getCurrentParagraphStyle();
    
    // 打开段落设置对话框
    ParagraphStyle newStyle = ParagraphDialog::getStyle(initialStyle, this);
    
    // 应用新样式
    m_formatController->applyParagraphStyle(newStyle);
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
    
    // 使用 DocumentScene 的新方法来更新光标，确保高度正确
    if (m_scene) {
        m_scene->updateCursorFromPosition(pos);
        
        // 同时获取视觉位置用于 View 的光标
        QPointF visualPos = calculateCursorVisualPosition(pos);
        m_view->setCursorVisualPosition(visualPos);
    }
    
    // 同时更新状态栏，显示光标位置
    updateStatusBar(m_lastScenePos, m_lastViewPos);
    
    // 注意：不再在这里更新样式状态，只在鼠标松开时更新
    // 无选区时的样式更新由单独的 cursor->positionChanged 连接处理
}

void MainWindow::updateStyleState()
{
    if (!m_ribbonBar || !m_document || !m_formatController) {
        return;
    }
    
    qDebug() << "MainWindow::updateStyleState - 开始更新样式";
    
    bool hasSelection = (m_selection && !m_selection->isEmpty());
    CharacterStyle style = m_formatController->getCurrentDisplayStyle();
    
    qDebug() << QString("加粗=%1, 斜体=%2, 下划线=%3, 有选区=%4")
        .arg(style.bold())
        .arg(style.italic())
        .arg(style.underline())
        .arg(hasSelection);
    
    if (!hasSelection) {
        // ========== 无选区：所有属性都一致 ==========
        m_ribbonBar->updateFromSelection(style, true);
        qDebug() << "  无选区，工具栏显示一致样式";
    } else {
        // ========== 先检查是否完全在单个 Span 内 ==========
        bool isSingleSpan = m_formatController->isSelectionStyleConsistent();
        
        if (isSingleSpan) {
            // 完全在单个 Span 内：所有属性都一致，直接显示
            m_ribbonBar->updateFromSelection(style, true);
            qDebug() << "  选区完全在单个 Span 内，工具栏显示一致样式";
        } else {
            // ========== 跨多个 Span：获取每个属性的一致性状态 ==========
            FormatController::StyleConsistency consistency = m_formatController->getSelectionStyleConsistency();
            
            // 将 FormatController::StyleConsistency 转换为 RibbonBar::StyleConsistency
            RibbonBar::StyleConsistency ribbonConsistency;
            ribbonConsistency.fontFamilyConsistent = consistency.fontFamilyConsistent;
            ribbonConsistency.fontSizeConsistent = consistency.fontSizeConsistent;
            ribbonConsistency.boldConsistent = consistency.boldConsistent;
            ribbonConsistency.italicConsistent = consistency.italicConsistent;
            ribbonConsistency.underlineConsistent = consistency.underlineConsistent;
            // 传递一致的属性值
            ribbonConsistency.consistentFontFamily = consistency.consistentFontFamily;
            ribbonConsistency.consistentFontSize = consistency.consistentFontSize;
            ribbonConsistency.consistentBold = consistency.consistentBold;
            ribbonConsistency.consistentItalic = consistency.consistentItalic;
            ribbonConsistency.consistentUnderline = consistency.consistentUnderline;
            
            m_ribbonBar->updateFromSelection(style, ribbonConsistency);
            qDebug() << "  选区跨多个 Span，根据各属性一致性更新工具栏";
        }
    }
    
    qDebug() << "  已更新到 ribbonBar";
}

QPointF MainWindow::calculateCursorVisualPosition(const CursorPosition &pos)
{
  //  QDebug() << "calculateCursorVisualPosition - 计算光标位置，位置:" << pos.blockIndex << "," << pos.offset;
    
    if (m_scene) {
        QPointF result = m_scene->calculateCursorVisualPosition(pos);
      //  QDebug() << "  返回场景坐标:" << result;
        return result;
    }
    
  //  QDebug() << "  场景为空，返回 (0,0)";
    return QPointF(0, 0);
}

void MainWindow::setupDebugConsole()
{
    m_debugConsoleDock = new QDockWidget(tr("调试控制台"), this);
    m_debugConsoleDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    m_debugConsoleDock->setMinimumWidth(500);  // 设置最小宽度为 400 像素，更宽一些
    
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
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    QPushButton *copyButton = new QPushButton(tr("复制"), dockContent);
    connect(copyButton, &QPushButton::clicked, this, [this]() {
        if (m_debugConsoleTextEdit->textCursor().hasSelection()) {
            m_debugConsoleTextEdit->copy();
        } else {
            m_debugConsoleTextEdit->selectAll();
            m_debugConsoleTextEdit->copy();
        }
    });
    buttonLayout->addWidget(copyButton);
    
    QPushButton *clearButton = new QPushButton(tr("清空"), dockContent);
    connect(clearButton, &QPushButton::clicked, this, [this]() {
        m_debugConsoleTextEdit->clear();
        DebugConsole::log(tr("调试控制台已清空"));
    });
    buttonLayout->addWidget(clearButton);
    
    dockLayout->addLayout(buttonLayout);
    
    dockContent->setLayout(dockLayout);
    m_debugConsoleDock->setWidget(dockContent);
    
    addDockWidget(Qt::RightDockWidgetArea, m_debugConsoleDock);
    
    // 连接 DebugConsole 的信号到我们的槽（支持彩色的新信号）
    connect(DebugConsole::instance(), QOverload<const LogMessage&>::of(&DebugConsole::logMessage),
            this, [this](const LogMessage &msg) {
        QTextCursor cursor = m_debugConsoleTextEdit->textCursor();
        cursor.movePosition(QTextCursor::End);
        
        QTextCharFormat format;
        format.setForeground(QBrush(msg.color));
        
        cursor.setCharFormat(format);
        cursor.insertText(msg.text + "\n");
        
        m_debugConsoleTextEdit->setTextCursor(cursor);
    });
    
    // 同时连接旧的信号以保持兼容（可选）
    connect(DebugConsole::instance(), QOverload<const QString&>::of(&DebugConsole::logMessage),
            this, [this](const QString &message) {
        // 我们已经通过新信号处理了，这里可以留空
        Q_UNUSED(message);
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

// ========== 辅助方法实现 ==========

void MainWindow::toggleStyleAttribute(
    const std::function<bool(const CharacterStyle&)>& getPropertyFunc,
    const std::function<void(CharacterStyle&, bool)>& setPropertyFunc,
    const std::function<void(bool)>& applyStyleFunc,
    const QString& styleName)
{
    qDebug() << "MainWindow:" << styleName << "按钮被点击";
    
    CharacterStyle style;
    if (m_selection->isEmpty()) {
        // 无选区：根据当前显示样式切换
        CharacterStyle currentStyle = m_formatController->getCurrentDisplayStyle();
        qDebug() << QString("  无选区，getCurrentDisplayStyle() 返回的%1:%2").arg(styleName).arg(getPropertyFunc(currentStyle));
        bool newValue = !getPropertyFunc(currentStyle);
        qDebug() << QString("  计算的新%1:%2").arg(styleName).arg(newValue);
        setPropertyFunc(style, newValue);
        m_formatController->setCurrentInputStyle(style);
    } else {
        // 有选区：检查选区内是否全部都是该样式
        bool allSet = false;
        if (styleName == "加粗") {
            allSet = m_formatController->isSelectionAllBold();
        } else if (styleName == "斜体") {
            allSet = m_formatController->isSelectionAllItalic();
        } else if (styleName == "下划线") {
            allSet = m_formatController->isSelectionAllUnderline();
        }
        
        qDebug() << QString("  有选区，选区内全部%1:%2").arg(styleName).arg(allSet);
        
        bool newValue;
        if (allSet) {
            // 选区内全部都是该样式 → 取消
            newValue = false;
            qDebug() << QString("  选区内全部%1，设置为 false").arg(styleName);
        } else {
            // 选区内不一致或未设置 → 设置为 true
            newValue = true;
            qDebug() << QString("  选区内不一致或未%1，设置为 true").arg(styleName);
        }
        
        setPropertyFunc(style, newValue);
        applyStyleFunc(newValue);
    }
    updateStyleState();
}

void MainWindow::applyFontProperty(
    const std::function<void(CharacterStyle&)>& setPropertyFunc,
    const QString& propertyName)
{
    qDebug() << "MainWindow:" << propertyName << "被调用";
    CharacterStyle style;
    if (m_selection->isEmpty()) {
        setPropertyFunc(style);
        m_formatController->setCurrentInputStyle(style);
    } else {
        setPropertyFunc(style);
        m_formatController->applyCharacterStyle(style);
    }
    updateStyleState();
}

void MainWindow::onStyleChanged()
{
    if (m_scene) {
        m_scene->updateAllTextItems();
    }
    // 刷新 RibbonBar 的样式列表
    if (m_ribbonBar) {
        m_ribbonBar->refreshStyleLists();
    }
}

void MainWindow::insertFractionAtCursor()
{
    if (!m_document || !m_cursor) {
        qDebug() << "[insertFractionAtCursor] 文档或光标无效";
        return;
    }

    // 步骤1：获取当前光标位置
    CursorPosition pos = m_cursor->position();
    qDebug() << "[insertFractionAtCursor] 当前光标位置：块" << pos.blockIndex << "，偏移" << pos.offset;

    // 步骤2：获取当前光标所在的 ParagraphBlock
    if (!m_document->sectionCount()) {
        qDebug() << "[insertFractionAtCursor] 文档没有section";
        return;
    }

    Section *section = m_document->section(0);
    if (!section) {
        qDebug() << "[insertFractionAtCursor] section无效";
        return;
    }

    if (pos.blockIndex < 0 || pos.blockIndex >= section->blockCount()) {
        qDebug() << "[insertFractionAtCursor] 块索引无效：" << pos.blockIndex;
        return;
    }

    Block *block = section->block(pos.blockIndex);
    if (!block) {
        qDebug() << "[insertFractionAtCursor] 块无效";
        return;
    }

    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(block);
    if (!para) {
        qDebug() << "[insertFractionAtCursor] 块不是ParagraphBlock";
        return;
    }

    // 步骤3：验证偏移位置是否有效
    int paraLength = para->length();
    if (pos.offset < 0 || pos.offset > paraLength) {
        qDebug() << "[insertFractionAtCursor] 偏移位置无效：" << pos.offset << "，段落长度：" << paraLength;
        return;
    }

    qDebug() << "[insertFractionAtCursor] 段落长度：" << paraLength;

    // 步骤4：创建分数 MathSpan
    // 创建分子（默认为"1"）
    NumberMathSpan *numSpan = new NumberMathSpan("1");
    // 创建分母（默认为"2"）
    NumberMathSpan *denSpan = new NumberMathSpan("2");
    // 创建分数
    FractionMathSpan *fracSpan = new FractionMathSpan(numSpan, denSpan);

    qDebug() << "[insertFractionAtCursor] 创建分数 MathSpan 成功";

    // 步骤5：将分数插入到光标位置
    para->insertInlineSpanAtPosition(pos.offset, fracSpan);

    qDebug() << "[insertFractionAtCursor] 分数插入成功，位置：" << pos.offset;

    // 步骤6：更新光标位置，移动到插入位置之后
    // MathSpan 占用 1 个字符位置
    CursorPosition newPos = pos;
    newPos.offset = pos.offset + 1;
    m_cursor->setPosition(newPos);

    qDebug() << "[insertFractionAtCursor] 光标已移动到：块" << newPos.blockIndex << "，偏移" << newPos.offset;

    // 步骤7：标记文档已修改
    m_isModified = true;
    updateWindowTitle();

    qDebug() << "[insertFractionAtCursor] 完成！";
}

void MainWindow::insertNumberAtCursor()
{
    if (!m_document || !m_cursor) {
        qDebug() << "[insertNumberAtCursor] 文档或光标无效";
        return;
    }

    // 步骤1：获取当前光标位置
    CursorPosition pos = m_cursor->position();
    qDebug() << "[insertNumberAtCursor] 当前光标位置：块" << pos.blockIndex << "，偏移" << pos.offset;

    // 步骤2：获取当前光标所在的 ParagraphBlock
    if (!m_document->sectionCount()) {
        qDebug() << "[insertNumberAtCursor] 文档没有section";
        return;
    }

    Section *section = m_document->section(0);
    if (!section) {
        qDebug() << "[insertNumberAtCursor] section无效";
        return;
    }

    if (pos.blockIndex < 0 || pos.blockIndex >= section->blockCount()) {
        qDebug() << "[insertNumberAtCursor] 块索引无效：" << pos.blockIndex;
        return;
    }

    Block *block = section->block(pos.blockIndex);
    if (!block) {
        qDebug() << "[insertNumberAtCursor] 块无效";
        return;
    }

    ParagraphBlock *para = qobject_cast<ParagraphBlock*>(block);
    if (!para) {
        qDebug() << "[insertNumberAtCursor] 块不是ParagraphBlock";
        return;
    }

    // 步骤3：验证偏移位置是否有效
    int paraLength = para->length();
    if (pos.offset < 0 || pos.offset > paraLength) {
        qDebug() << "[insertNumberAtCursor] 偏移位置无效：" << pos.offset << "，段落长度：" << paraLength;
        return;
    }

    qDebug() << "[insertNumberAtCursor] 段落长度：" << paraLength;

    // 步骤4：创建空的 NumberMathSpan
    NumberMathSpan *numSpan = new NumberMathSpan("");

    qDebug() << "[insertNumberAtCursor] 创建 NumberMathSpan 成功";

    // 步骤5：将公式文本插入到光标位置
    para->insertInlineSpanAtPosition(pos.offset, numSpan);

    qDebug() << "[insertNumberAtCursor] 公式文本插入成功，位置：" << pos.offset;

    // 步骤6：更新光标位置，移动到插入位置之后
    // MathSpan 占用 1 个字符位置
    CursorPosition newPos = pos;
    newPos.offset = pos.offset + 1;
    m_cursor->setPosition(newPos);

    qDebug() << "[insertNumberAtCursor] 光标已移动到：块" << newPos.blockIndex << "，偏移" << newPos.offset;

    // 步骤7：标记文档已修改
    m_isModified = true;
    updateWindowTitle();

    qDebug() << "[insertNumberAtCursor] 完成！";
}

} // namespace QtWordEditor
