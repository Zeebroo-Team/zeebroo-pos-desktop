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

private slots:
    void addNewSession();
    void closeSession(int index);

private:
    PosSessionWidget *currentSession() const;
    void updateTabTitle(int index, PosSessionWidget *session);

    ApiClient *m_api = nullptr;
    QTabWidget *m_tabs = nullptr;
    int m_sessionCounter = 0;
};

} // namespace pos
