#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>

#include <QFile>
#include <QSaveFile>

#include <QSettings>

#include <QSortFilterProxyModel>

#include "blocktablemodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const QString &path = "" ,QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpen_triggered();

    void on_actionExport_triggered();

    void on_comboBoxGameMode_activated(int index);

    void on_actionImport_triggered();

    void on_actionSave_triggered();

    void on_actionAbout_triggered();

    void setBlockFile(const QString &path);

private:
    Ui::MainWindow *ui;

    BlockTableModel *m_model;
    QSortFilterProxyModel *m_proxyModel;

    QString m_path;

    QSettings *m_settings;

    const QString product_version = "0.1";
    const QString product_site = "";
};
#endif // MAINWINDOW_H
