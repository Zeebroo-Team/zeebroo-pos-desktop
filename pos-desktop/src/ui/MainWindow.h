#pragma once

#include "core/ApiClient.h"
#include <QMainWindow>

class QTabWidget;

namespace pos {

class PosSessionWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)
public:
    explicit MainWindow(ApiClient *api, QWidget *parent = nullptr);

    bool wasSignedOut() const { return m_signedOut; }

private slots:
    void addNewSession();
    void closeSession(int index);
    void showKeyboardShortcuts();

private:
    void buildMenuBar();
    PosSessionWidget *currentSession() const;
    void updateTabTitle(int index, PosSessionWidget *session);

    ApiClient  *m_api           = nullptr;
    QTabWidget *m_tabs          = nullptr;
    int         m_sessionCounter = 0;
    bool        m_signedOut      = false;
};

} // namespace pos
