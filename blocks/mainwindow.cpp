#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(const QString &path, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Model
    m_model = new BlockTableModel(this);

    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);

    m_proxyModel->setFilterKeyColumn(6);

    ui->tableView->setModel(m_proxyModel);

    ui->tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableView->setColumnHidden(6, true);

    ui->tableView->addActions(ui->menuEdit->actions());

    //
    m_settings = new QSettings(QApplication::applicationDirPath() +
                               "/" +
                               QApplication::applicationName() +
                               ".ini",
                               QSettings::IniFormat,
                               this);

    if (path != "")
        setBlockFile(path);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered()
{
    const QString filter = tr("All supported formats") + "(MISSION.BLK MISSIONS.BLK)"
                                                         ";;Driver(MISSION.BLK)"
                                                         ";;Driver 2(MISSIONS.BLK)";

    const QString path = QFileDialog::getOpenFileName(this,
                                                      tr("Open file"),
                                                      m_settings->value("block_path").toString(),
                                                      filter);

    if (path == "") {
        return;
    }

    m_path = "";
    setWindowTitle("Blocks");

    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("Can't open file in read mode") +
                              " " +
                              QDir::toNativeSeparators(m_path));
        return;
    }

    const QByteArray data = file.readAll();

    file.close();

    //Driver 2 mission names
    QDir dir(QFileInfo(path).absolutePath());
    dir.cdUp();

    file.setFileName(dir.path() + "/LANG/EN_MISSION.LTXT");

    QString missionNames;

    if (file.open(QIODevice::ReadOnly))
        missionNames = file.readAll();

    if (!m_model->setBlockData(data, missionNames)) {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("Can't parse data") +
                              " " +
                              QDir::toNativeSeparators(path));
        return;
    }

    //Filter
    ui->comboBoxGameMode->clear();
    ui->comboBoxGameMode->addItems(m_model->gameModeList());

    int value = m_settings->value("latest_game_mode", "0").toInt();
    int index = (value > -1 && value < ui->comboBoxGameMode->count()) ? value : 0;

    ui->comboBoxGameMode->setCurrentIndex(index);
    emit ui->comboBoxGameMode->activated(ui->comboBoxGameMode->currentIndex());

    //
    m_path = path;

    setWindowTitle(QDir::toNativeSeparators(path));

    m_settings->setValue("block_path", m_path);
}


void MainWindow::on_actionExport_triggered()
{
    QModelIndex index = ui->tableView->currentIndex();

    if (index.isValid()) {
        index = m_proxyModel->mapToSource(index);

        const QString extension = (m_model->blockType() == Block::Driver) ? ".cdm" : ".d2ms";

        const QString path = QFileDialog::getSaveFileName(this,
                                                          tr("Save"),
                                                          m_settings->value("export_path").toString() +
                                                          "/" +
                                                          QString().setNum(index.row()) +
                                                          extension,
                                                          "Mission (*" + extension + ")");

        if (path == "") {
            return;
        }

        QSaveFile saveFile(path);

        if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("Can't open file in write mode") +
                                  " " +
                                  QDir::toNativeSeparators(path));
            return;
        }

        const QByteArray *data = m_model->missionData(index);

        if (data != nullptr && (saveFile.write(*data) == -1 || !saveFile.commit())) {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  tr("Can't write file") +
                                  " " +
                                  QDir::toNativeSeparators(path));
            return;
        }

        m_settings->setValue("export_path", QFileInfo(path).absolutePath());
    }
}


void MainWindow::on_comboBoxGameMode_activated(int index)
{
    if (index == 0) {
        m_proxyModel->setFilterFixedString("");
    }
    else {
        m_proxyModel->setFilterRegExp("^" + QString().setNum(index) + "$");
    }

    m_settings->setValue("latest_game_mode", ui->comboBoxGameMode->currentIndex());
}


void MainWindow::on_actionImport_triggered()
{
    QModelIndex index = ui->tableView->currentIndex();

    if (index.isValid()) {
        index = m_proxyModel->mapToSource(index);

        const QString extension = (m_model->blockType() == Block::Driver) ? ".cdm" : ".d2ms";

        const QString path = QFileDialog::getOpenFileName(this,
                                                          "Open",
                                                          m_settings->value("import_path").toString() +
                                                          "/" +
                                                          QString().setNum(index.row()) +
                                                          extension,
                                                          "Mission (*" + extension + ")");

        if (path == "") {
            return;
        }

        setBlockFile(path);
    }
}

void MainWindow::on_actionSave_triggered()
{
    if (m_path == "")
        return;

    QSaveFile saveFile(m_path);

    if (!saveFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("Can't open file in write mode") +
                              " " +
                              QDir::toNativeSeparators(m_path));
        return;
    }

    const QByteArray data = m_model->blockData();

    if ((saveFile.write(data) == -1) || !saveFile.commit()) {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("Can't write file") +
                              " " +
                              QDir::toNativeSeparators(m_path));
        return;
    }

    setWindowTitle(QDir::toNativeSeparators(m_path));
}



void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::information(this,
                             tr("About"),
                             tr("Version") + ": " + product_version + "<br/>" +
                             tr("Site") + ": " + "<a href=" + product_site + ">" + product_site + "</a>");
}

void MainWindow::setBlockFile(const QString &path)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("Can't open file in read mode") +
                              " " +
                              QDir::toNativeSeparators(m_path));
        return;
    }

    const QByteArray data = file.readAll();

    file.close();

    //Driver 2 mission names
    QDir dir(QFileInfo(path).absolutePath());
    dir.cdUp();

    file.setFileName(dir.path() + "/LANG/EN_MISSION.LTXT");

    QString missionNames;

    if (file.open(QIODevice::ReadOnly))
        missionNames = file.readAll();

    if (!m_model->setBlockData(data, missionNames)) {
        QMessageBox::critical(this,
                              tr("Error"),
                              tr("Can't parse data") +
                              " " +
                              QDir::toNativeSeparators(path));
        return;
    }

    //Filter
    ui->comboBoxGameMode->clear();
    ui->comboBoxGameMode->addItems(m_model->gameModeList());

    int value = m_settings->value("latest_game_mode", "0").toInt();
    int index = (value > -1 && value < ui->comboBoxGameMode->count()) ? value : 0;

    ui->comboBoxGameMode->setCurrentIndex(index);
    emit ui->comboBoxGameMode->activated(ui->comboBoxGameMode->currentIndex());

    //
    m_path = path;

    setWindowTitle(QDir::toNativeSeparators(path));

    m_settings->setValue("block_path", m_path);
}

