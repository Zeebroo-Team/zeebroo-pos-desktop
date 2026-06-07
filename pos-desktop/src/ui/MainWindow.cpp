#include "ui/MainWindow.h"
#include "ui/PosSessionWidget.h"

#include <QHBoxLayout>
#include <QLabel>
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
    m_tabs->setTabsClosable(false); // start with 1 tab — no close button needed
    m_tabs->setMovable(true);
    m_tabs->setDocumentMode(false);

    // "＋ New Session" corner button
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

    // ── First session ─────────────────────────────────────────────────────────
    addNewSession();

    // ── Shortcuts: tab management ─────────────────────────────────────────────
    auto *scNew = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_T), this);
    connect(scNew, &QShortcut::activated, this, &MainWindow::addNewSession);

    auto *scClose = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_W), this);
    connect(scClose, &QShortcut::activated, this, [this]() {
        closeSession(m_tabs->currentIndex());
    });

    for (int i = 1; i <= 9; ++i) {
        auto *sc = new QShortcut(QKeySequence(Qt::CTRL | (Qt::Key_0 + i)), this);
        connect(sc, &QShortcut::activated, this, [this, i]() {
            if (i - 1 < m_tabs->count()) m_tabs->setCurrentIndex(i - 1);
        });
    }

    // ── Shortcuts: delegate F-keys to active session ──────────────────────────
    auto delegate = [this](Qt::Key key, void (PosSessionWidget::*fn)()) {
        auto *sc = new QShortcut(QKeySequence(key), this);
        connect(sc, &QShortcut::activated, this, [this, fn]() {
            if (auto *s = currentSession()) (s->*fn)();
        });
    };

    delegate(Qt::Key_F2,  &PosSessionWidget::focusSearch);
    delegate(Qt::Key_F3,  &PosSessionWidget::focusSku);
    delegate(Qt::Key_F4,  &PosSessionWidget::triggerAddProduct);
    delegate(Qt::Key_F5,  &PosSessionWidget::triggerRefresh);
    delegate(Qt::Key_F6,  &PosSessionWidget::triggerClearFilters);
    delegate(Qt::Key_F8,  &PosSessionWidget::triggerClearCart);
    delegate(Qt::Key_F9,  &PosSessionWidget::triggerReturn);
    delegate(Qt::Key_F12, &PosSessionWidget::triggerCheckout);

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
            tr("Your session expired. Restart the app to sign in again."));
    });
}

// ── Tab management ─────────────────────────────────────────────────────────────

void MainWindow::addNewSession()
{
    ++m_sessionCounter;
    auto *session = new PosSessionWidget(m_api, m_sessionCounter, m_tabs);
    const QString name = tr("Session %1").arg(m_sessionCounter);
    const int idx = m_tabs->addTab(session, name);
    m_tabs->setCurrentIndex(idx);

    // Show close buttons once there are ≥2 tabs
    m_tabs->setTabsClosable(m_tabs->count() > 1);

    connect(session, &PosSessionWidget::sessionCartChanged, this, [this, session]() {
        updateTabTitle(m_tabs->indexOf(session), session);
    });
}

void MainWindow::closeSession(int index)
{
    if (m_tabs->count() <= 1) return; // keep at least one session

    QWidget *w = m_tabs->widget(index);
    m_tabs->removeTab(index);
    w->deleteLater();

    // Hide close buttons when only one tab remains
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

} // namespace pos
