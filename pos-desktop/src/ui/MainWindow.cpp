#include "ui/MainWindow.h"
#include "ui/PosSessionWidget.h"
#include "ui/PurchaseOrderDialog.h"

#include <QAction>
#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>
#include <QTabWidget>
#include <QVBoxLayout>

namespace pos {

MainWindow::MainWindow(ApiClient *api, QWidget *parent)
    : QMainWindow(parent)
    , m_api(api)
{
    setWindowTitle(tr("Zeebroo POS"));
    resize(1280, 800);

    auto *central = new QWidget(this);
    central->setObjectName(QStringLiteral("centralWidget"));
    auto *root = new QVBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Tab widget ────────────────────────────────────────────────────────────
    m_tabs = new QTabWidget(central);
    m_tabs->setObjectName(QStringLiteral("posTabWidget"));
    m_tabs->setTabsClosable(false);
    m_tabs->setMovable(true);
    m_tabs->setDocumentMode(false);

    auto *addTabBtn = new QPushButton(tr("  ＋  New Session"), central);
    addTabBtn->setObjectName(QStringLiteral("addSessionBtn"));
    addTabBtn->setCursor(Qt::PointingHandCursor);
    addTabBtn->setFixedHeight(28);
    addTabBtn->setToolTip(tr("Open a new POS session  [Ctrl+T]"));
    m_tabs->setCornerWidget(addTabBtn, Qt::TopRightCorner);
    connect(addTabBtn, &QPushButton::clicked, this, &MainWindow::addNewSession);
    connect(m_tabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeSession);

    root->addWidget(m_tabs, 1);

    // ── Hint bar ──────────────────────────────────────────────────────────────
    auto *hintBar = new QLabel(
        tr("F2  Search   ·   F3  SKU / Scan   ·   F4  New Product   ·   F5  Refresh   ·   "
           "F6  Clear   ·   F8  Clear Cart   ·   F9  Return   ·   F12  Checkout   ·   "
           "Ctrl+◄►  Category   ·   Ctrl+T  New Session   ·   Ctrl+W  Close Session   ·   Ctrl+1–9  Switch"),
        central);
    hintBar->setObjectName(QStringLiteral("shortcutBar"));
    hintBar->setAlignment(Qt::AlignCenter);
    root->addWidget(hintBar);

    setCentralWidget(central);

    // ── Menu bar (builds QAction shortcuts — no separate QShortcuts for these) ─
    buildMenuBar();

    // ── First session ─────────────────────────────────────────────────────────
    addNewSession();

    // ── Shortcuts: session switching Ctrl+1–9 ─────────────────────────────────
    for (int i = 1; i <= 9; ++i) {
        auto *sc = new QShortcut(QKeySequence(Qt::CTRL | (Qt::Key_0 + i)), this);
        connect(sc, &QShortcut::activated, this, [this, i]() {
            if (i - 1 < m_tabs->count()) m_tabs->setCurrentIndex(i - 1);
        });
    }

    // ── Shortcuts: category navigation Ctrl+Left / Right ─────────────────────
    auto *scPrevCat = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Left), this);
    connect(scPrevCat, &QShortcut::activated, this, [this]() {
        if (auto *s = currentSession()) s->navigatePreviousCategory();
    });

    auto *scNextCat = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Right), this);
    connect(scNextCat, &QShortcut::activated, this, [this]() {
        if (auto *s = currentSession()) s->navigateNextCategory();
    });

    // ── Global error handler ──────────────────────────────────────────────────
    connect(m_api, &ApiClient::unauthorized, this, [this]() {
        QMessageBox::warning(this, tr("Session expired"),
            tr("Your session expired. Please sign in again."));
    });
}

// ── Menu bar ───────────────────────────────────────────────────────────────────

void MainWindow::buildMenuBar()
{
    // ── File ─────────────────────────────────────────────────────────────────
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    auto *actNewSession = new QAction(tr("New Session"), this);
    actNewSession->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_T));
    actNewSession->setStatusTip(tr("Open a new POS session"));
    connect(actNewSession, &QAction::triggered, this, &MainWindow::addNewSession);
    fileMenu->addAction(actNewSession);

    auto *actCloseSession = new QAction(tr("Close Session"), this);
    actCloseSession->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_W));
    actCloseSession->setStatusTip(tr("Close the current POS session"));
    connect(actCloseSession, &QAction::triggered, this, [this]() {
        closeSession(m_tabs->currentIndex());
    });
    fileMenu->addAction(actCloseSession);

    fileMenu->addSeparator();

    auto *actSignOut = new QAction(tr("Sign Out…"), this);
    actSignOut->setStatusTip(tr("Sign out and return to the login screen"));
    connect(actSignOut, &QAction::triggered, this, [this]() {
        if (QMessageBox::question(this, tr("Sign Out"),
                tr("Are you sure you want to sign out?"),
                QMessageBox::Yes | QMessageBox::Cancel,
                QMessageBox::Cancel) == QMessageBox::Yes) {
            m_signedOut = true;
            close();
        }
    });
    fileMenu->addAction(actSignOut);

    fileMenu->addSeparator();

    auto *actExit = new QAction(tr("Exit"), this);
    actExit->setMenuRole(QAction::QuitRole);
    actExit->setShortcut(QKeySequence::Quit);
    actExit->setStatusTip(tr("Quit Zeebroo POS"));
    connect(actExit, &QAction::triggered, this, [this]() { close(); });
    fileMenu->addAction(actExit);

    // ── View ──────────────────────────────────────────────────────────────────
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    auto *actRefresh = new QAction(tr("Refresh Catalog"), this);
    actRefresh->setShortcut(QKeySequence(Qt::Key_F5));
    actRefresh->setStatusTip(tr("Reload the product catalog from the server"));
    connect(actRefresh, &QAction::triggered, this, [this]() {
        if (auto *s = currentSession()) s->triggerRefresh();
    });
    viewMenu->addAction(actRefresh);

    auto *actClearFilters = new QAction(tr("Clear Filters"), this);
    actClearFilters->setShortcut(QKeySequence(Qt::Key_F6));
    actClearFilters->setStatusTip(tr("Clear the search and category filters"));
    connect(actClearFilters, &QAction::triggered, this, [this]() {
        if (auto *s = currentSession()) s->triggerClearFilters();
    });
    viewMenu->addAction(actClearFilters);

    viewMenu->addSeparator();

    auto *actFullscreen = new QAction(tr("Full Screen"), this);
    actFullscreen->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_F));
    actFullscreen->setCheckable(true);
    actFullscreen->setStatusTip(tr("Toggle full-screen mode"));
    connect(actFullscreen, &QAction::toggled, this, [this](bool on) {
        if (on) showFullScreen();
        else    showMaximized();
    });
    viewMenu->addAction(actFullscreen);

    // ── POS ───────────────────────────────────────────────────────────────────
    QMenu *posMenu = menuBar()->addMenu(tr("&POS"));

    auto *actSearch = new QAction(tr("Search Products"), this);
    actSearch->setShortcut(QKeySequence(Qt::Key_F2));
    actSearch->setStatusTip(tr("Focus the product search box"));
    connect(actSearch, &QAction::triggered, this, [this]() {
        if (auto *s = currentSession()) s->focusSearch();
    });
    posMenu->addAction(actSearch);

    auto *actSku = new QAction(tr("Scan SKU / Barcode"), this);
    actSku->setShortcut(QKeySequence(Qt::Key_F3));
    actSku->setStatusTip(tr("Focus the SKU / barcode scanner field"));
    connect(actSku, &QAction::triggered, this, [this]() {
        if (auto *s = currentSession()) s->focusSku();
    });
    posMenu->addAction(actSku);

    auto *actAddProduct = new QAction(tr("Add Product…"), this);
    actAddProduct->setShortcut(QKeySequence(Qt::Key_F4));
    actAddProduct->setStatusTip(tr("Open the add-product dialog"));
    connect(actAddProduct, &QAction::triggered, this, [this]() {
        if (auto *s = currentSession()) s->triggerAddProduct();
    });
    posMenu->addAction(actAddProduct);

    posMenu->addSeparator();

    auto *actPurchaseOrders = new QAction(tr("Purchase Orders…"), this);
    actPurchaseOrders->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_P));
    actPurchaseOrders->setStatusTip(tr("View and manage purchase orders"));
    connect(actPurchaseOrders, &QAction::triggered, this, &MainWindow::openPurchaseOrders);
    posMenu->addAction(actPurchaseOrders);

    posMenu->addSeparator();

    auto *actClearCart = new QAction(tr("Clear Cart"), this);
    actClearCart->setShortcut(QKeySequence(Qt::Key_F8));
    actClearCart->setStatusTip(tr("Remove all items from the current cart"));
    connect(actClearCart, &QAction::triggered, this, [this]() {
        if (auto *s = currentSession()) s->triggerClearCart();
    });
    posMenu->addAction(actClearCart);

    auto *actReturn = new QAction(tr("Return / Refund…"), this);
    actReturn->setShortcut(QKeySequence(Qt::Key_F9));
    actReturn->setStatusTip(tr("Process a product return or refund"));
    connect(actReturn, &QAction::triggered, this, [this]() {
        if (auto *s = currentSession()) s->triggerReturn();
    });
    posMenu->addAction(actReturn);

    auto *actCheckout = new QAction(tr("Checkout"), this);
    actCheckout->setShortcut(QKeySequence(Qt::Key_F12));
    actCheckout->setStatusTip(tr("Open the checkout / payment dialog"));
    connect(actCheckout, &QAction::triggered, this, [this]() {
        if (auto *s = currentSession()) s->triggerCheckout();
    });
    posMenu->addAction(actCheckout);

    // ── Help ──────────────────────────────────────────────────────────────────
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    auto *actShortcuts = new QAction(tr("Keyboard Shortcuts"), this);
    actShortcuts->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_Slash));
    actShortcuts->setStatusTip(tr("Show all keyboard shortcuts"));
    connect(actShortcuts, &QAction::triggered, this, &MainWindow::showKeyboardShortcuts);
    helpMenu->addAction(actShortcuts);

    helpMenu->addSeparator();

    auto *actAbout = new QAction(tr("About Zeebroo POS"), this);
    actAbout->setMenuRole(QAction::AboutRole);
    actAbout->setStatusTip(tr("About this application"));
    connect(actAbout, &QAction::triggered, this, [this]() {
        QMessageBox::about(this, tr("About Zeebroo POS"),
            tr("<h3>Zeebroo POS</h3>"
               "<p>Version 1.0</p>"
               "<p>A modern point-of-sale system powered by the Zeebroo platform.</p>"
               "<p>© 2024 Zeebroo. All rights reserved.</p>"));
    });
    helpMenu->addAction(actAbout);
}

void MainWindow::showKeyboardShortcuts()
{
    QMessageBox box(this);
    box.setWindowTitle(tr("Keyboard Shortcuts"));
    box.setTextFormat(Qt::RichText);
    box.setText(
        tr("<table cellpadding='6'>"
           "<tr>"
           "  <th align='left' style='padding-bottom:4px;border-bottom:1px solid #ddd'>Action</th>"
           "  <th align='left' style='padding-bottom:4px;border-bottom:1px solid #ddd;padding-left:24px'>Shortcut</th>"
           "</tr>"
           "<tr><td>Search products</td>     <td style='padding-left:24px'><b>F2</b></td></tr>"
           "<tr><td>Scan SKU / barcode</td>  <td style='padding-left:24px'><b>F3</b></td></tr>"
           "<tr><td>Add product</td>         <td style='padding-left:24px'><b>F4</b></td></tr>"
           "<tr><td>Refresh catalog</td>     <td style='padding-left:24px'><b>F5</b></td></tr>"
           "<tr><td>Clear filters</td>       <td style='padding-left:24px'><b>F6</b></td></tr>"
           "<tr><td>Clear cart</td>          <td style='padding-left:24px'><b>F8</b></td></tr>"
           "<tr><td>Return / refund</td>     <td style='padding-left:24px'><b>F9</b></td></tr>"
           "<tr><td>Checkout</td>            <td style='padding-left:24px'><b>F12</b></td></tr>"
           "<tr><td colspan='2'>&nbsp;</td></tr>"
           "<tr><td>New session</td>         <td style='padding-left:24px'><b>Ctrl+T</b></td></tr>"
           "<tr><td>Close session</td>       <td style='padding-left:24px'><b>Ctrl+W</b></td></tr>"
           "<tr><td>Switch session 1–9</td>  <td style='padding-left:24px'><b>Ctrl+1 … Ctrl+9</b></td></tr>"
           "<tr><td>Previous category</td>   <td style='padding-left:24px'><b>Ctrl+←</b></td></tr>"
           "<tr><td>Next category</td>       <td style='padding-left:24px'><b>Ctrl+→</b></td></tr>"
           "<tr><td>Full screen</td>         <td style='padding-left:24px'><b>Ctrl+Shift+F</b></td></tr>"
           "<tr><td>Keyboard shortcuts</td>  <td style='padding-left:24px'><b>Ctrl+/</b></td></tr>"
           "</table>"));
    box.exec();
}

// ── Tab management ─────────────────────────────────────────────────────────────

void MainWindow::addNewSession()
{
    ++m_sessionCounter;
    auto *session = new PosSessionWidget(m_api, m_sessionCounter, m_tabs);
    const QString name = tr("Session %1").arg(m_sessionCounter);
    const int idx = m_tabs->addTab(session, name);
    m_tabs->setCurrentIndex(idx);

    m_tabs->setTabsClosable(m_tabs->count() > 1);

    connect(session, &PosSessionWidget::sessionCartChanged, this, [this, session]() {
        updateTabTitle(m_tabs->indexOf(session), session);
    });
}

void MainWindow::closeSession(int index)
{
    if (m_tabs->count() <= 1) return;

    QWidget *w = m_tabs->widget(index);
    m_tabs->removeTab(index);
    w->deleteLater();

    m_tabs->setTabsClosable(m_tabs->count() > 1);
}

PosSessionWidget *MainWindow::currentSession() const
{
    return qobject_cast<PosSessionWidget *>(m_tabs->currentWidget());
}

void MainWindow::updateTabTitle(int index, PosSessionWidget *session)
{
    if (index < 0) return;
    const int count = session->cartItemCount();
    const QString base = tr("Session %1").arg(session->sessionNumber());
    m_tabs->setTabText(index, count > 0 ? tr("%1  (%2)").arg(base).arg(count) : base);
}

void MainWindow::openPurchaseOrders()
{
    PosSessionWidget *session = currentSession();
    if (!session) return;

    auto *dlg = new PurchaseOrderDialog(m_api, session->bootstrap(), this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->exec();
}

} // namespace pos
