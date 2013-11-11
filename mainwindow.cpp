#include <QtGui>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>

#include "sourceinfodialog.h"
#include "fileobject_info_dialog.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

// Limit to 5MB the file size for preview
#define MAX_SIZE_FOR_PREVIEW	(5 * 1024 * 1024)

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	m_model(0),
	m_proxyModel(0)
{
	ui->setupUi(this);

	ui->actionDfxmlSave->setEnabled(false);
	ui->actionObjectSave->setEnabled(false);
	ui->actionObjectInfo->setEnabled(false);
	ui->actionObjectView->setEnabled(false);
	ui->actionSaveSectorMap->setEnabled(false);
	ui->actionSourceInfo->setEnabled(false);
	ui->previewCheckBox->setEnabled(false);

	ui->textBrowser->setVisible(false);
	ui->previewLabel->setVisible(false);

	ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui->listView, SIGNAL(customContextMenuRequested(const QPoint &)),
		this, SLOT(showContextMenu(const QPoint &)));

	// connect dfxml actions
	connect(ui->actionDfxmlOpen, SIGNAL(triggered()), this, SLOT(onDfxmlLoad()));
	connect(ui->actionDfxmlSave, SIGNAL(triggered()), this, SLOT(onDfxmlSave()));

	// connect objects actions
	connect(ui->actionObjectSave, SIGNAL(triggered()), this, SLOT(onObjectSave()));
	connect(ui->actionObjectInfo, SIGNAL(triggered()), this, SLOT(onObjectInfo()));
	connect(ui->actionObjectView, SIGNAL(triggered()), this, SLOT(onObjectView()));
	connect(ui->actionObjectFilter, SIGNAL(triggered()), this, SLOT(onObjectViewFilter()));

	// connect source actions
	connect(ui->actionSourceInfo, SIGNAL(triggered()), this, SLOT(onSourceInfo()));
	connect(ui->actionSaveSectorMap, SIGNAL(triggered()), this, SLOT(onSaveSectorMap()));

	connect(ui->splitter, SIGNAL(splitterMoved(int, int)),
		this, SLOT(onSplitterMoved(int, int)));

	// connect other actions
	connect(ui->previewCheckBox, SIGNAL(toggled(bool)),
		this, SLOT(onPreviewToggled(bool)));

	connect(ui->listView, SIGNAL(clicked(const QModelIndex &)),
		this, SLOT(onListViewClicked(const QModelIndex &)));

	connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
}

// Context menu on file-object entries
void MainWindow::showContextMenu(const QPoint &pos)
{
	QMenu contextMenu(tr("Context menu"), this);
	contextMenu.addAction(ui->actionObjectSave);
	contextMenu.addAction(ui->actionObjectView);
	contextMenu.addAction(ui->actionObjectInfo);
	contextMenu.exec(mapToGlobal(pos));
}

MainWindow::~MainWindow()
{
	delete ui;

	if (m_proxyModel) {
		delete m_proxyModel;
		m_proxyModel = 0;
	}
	if (m_model) {
		delete m_model;
		m_model = 0;
	}

	// Remove all temp files.
	if (m_tmpFiles.count() > 0) {
		for (int i = 0; i < m_tmpFiles.count(); i++) {
			//qDebug() << i << " - Delete tmpfile:" << m_tmpFiles.at(i)->fileName();
			delete m_tmpFiles[i];
		}
	}
}

bool MainWindow::checkSourceFile()
{
	// Check source file
	QFile source(m_model->source()->imageName());
	if (source.exists()) {
		return true;
	}

	if (QMessageBox::question(this, "Source File Not Found!",
			tr("%1 Not Found!\n\n'Press Open' to select file or\n'Cancel' to continue without a source file.")
			.arg(m_model->source()->imageName()),
			 QMessageBox::Open,
			 QMessageBox::Cancel) == QMessageBox::Open)
	{
		QString sourceFile;
		sourceFile = QFileDialog::getOpenFileName(this, tr("Select Source"), QDir::currentPath(), tr("All Files (*.*)"));
		if (!sourceFile.isEmpty()) {
			m_model->source()->setImageFileName(sourceFile);
			return true;
		}
	}

	QMessageBox::warning(this, tr("No Source File"), tr("File Object Preview and Save are disabled!"));
	return false;
}

void MainWindow::onDfxmlLoad()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		 tr("Open Report"), QDir::currentPath(), tr("XML Files (*.xml)"));

	if (fileName.isEmpty())
		return;

	QFile file(fileName);
	if (!file.open(QFile::ReadOnly | QFile::Text)) {
		QMessageBox::warning(this, tr("DfXml"),
                              tr("Cannot read file %1:\n%2.")
                              .arg(fileName)
                              .arg(file.errorString()));
		return;
	}

	if (m_model) {
		delete m_model;
	}

	m_model = new DfxmlListModel();
	if (!m_model->setXmlFile(fileName)) {
		QMessageBox::warning(this, tr("Dfxml"), tr("Error parsing %1").arg(fileName));
		return;
	}

	m_currentFile = fileName;
	statusBar()->showMessage(tr("%1: [LOADED]").arg(fileName), 2000);

	// Check source file
	if (checkSourceFile()) {
		ui->actionObjectSave->setEnabled(true);
		ui->previewCheckBox->setEnabled(true);
	} else {
		ui->actionObjectSave->setEnabled(false);
		ui->previewCheckBox->setEnabled(false);
	}

	ui->sourceLabel->setText(m_model->sourceFileName());

	// Create a QFilterProxyModel for sort & filter the view
	if (m_proxyModel) {
		delete m_proxyModel;
	}

	m_proxyModel = new QSortFilterProxyModel(this);
	m_proxyModel->setSourceModel(m_model);
	m_proxyModel->sort(0, Qt::AscendingOrder);

	ui->listView->setSelectionRectVisible(true);
	ui->listView->setModel(m_proxyModel);
	ui->listView->setSelectionMode(QAbstractItemView::SingleSelection);
	m_selectionModel = ui->listView->selectionModel();

	connect(m_selectionModel,
		SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
		this,
		SLOT(onFileObjectChanged(const QModelIndex &, const QModelIndex &)));

	connect(ui->filterLineEdit, SIGNAL(textChanged(const QString &)),
		m_proxyModel, SLOT(setFilterRegExp(const QString &)));

	// Enable Actions
	ui->actionDfxmlSave->setEnabled(true);
	ui->actionObjectInfo->setEnabled(true);
	ui->actionObjectView->setEnabled(true);
	ui->actionSourceInfo->setEnabled(true);
	ui->actionSaveSectorMap->setEnabled(true);
}

void MainWindow::onDfxmlSave()
{
	QFileInfo fi(m_currentFile);
	QString fname = fi.baseName() + "_saved.xml";
	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                            fname, tr("XML Files (*.xml)"));

	if (fileName.isEmpty()) {
		return;
	}

	if (!m_model->saveXmlReport(fileName)) {
		QMessageBox::warning(this, tr("Save DfXml"),
                              tr("Cannot save file %1")
                              .arg(fileName));
	} else {
		statusBar()->showMessage(tr("%1 [SAVED]").arg(fileName), 2000);
	}
}

void MainWindow::onObjectSave()
{
	QModelIndex index = m_proxyModel->mapToSource(ui->listView->currentIndex());
	if (!index.isValid()) {
		qDebug() << __PRETTY_FUNCTION__	 << "Invalid ModelIndex";
		return;
	}

	QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                            m_model->fileObjectName(index));

	if (fileName.isEmpty()) {
		return;
	}

	QString err;
	if (!m_model->fileObjectSave(index, fileName, err)) {
		QMessageBox::warning(this, tr("DfXml"), err);
	} else {
		statusBar()->showMessage(tr("%1 [SAVED]").arg(fileName), 2000);
	}
}

// Rudimental preview!!! may explode with huge files so limit to MAX_SIZE_FOR_PREVIEW
// Model index is already mapped to source model
void MainWindow::updatePreview(const QModelIndex &index)
{
	if (ui->previewCheckBox->isChecked()) {
		qint64 size = m_model->fileObjectSize(index);
		if (size > MAX_SIZE_FOR_PREVIEW) {
			ui->textBrowser->setVisible(false);
			ui->previewLabel->setText("File too big for embedded preview!");
			ui->previewLabel->setVisible(true);
			return;
		}

		QPixmap pix;
		if (pix.loadFromData(m_model->fileObjectData(index))) {
			ui->textBrowser->setVisible(false);
			ui->dimensionLabel->setText(QString("%1 x %2").arg(pix.width()).arg(pix.height()));
			if ((pix.width() > ui->previewLabel->width()) || (pix.height() > ui->previewLabel->height())) {
				pix = pix.scaled(ui->previewLabel->width(), ui->previewLabel->height(), Qt::KeepAspectRatio, Qt::FastTransformation);
			}
			ui->previewLabel->setPixmap(pix);
			ui->previewLabel->setVisible(true);
		} else {
			// Not a Pixmap or broken Pixmap
			ui->previewLabel->setVisible(false);
			ui->textBrowser->setText(m_model->fileObjectData(index).data());
			ui->textBrowser->setVisible(true);
		}
	}
}

// SLOT [Private]
void MainWindow::onFileObjectChanged(const QModelIndex &current, const QModelIndex &previous)
{
	Q_UNUSED(previous)

	// Update preview only when index changed!
	QModelIndex mi = m_proxyModel->mapToSource(current);

	ui->objectFileLabel->setText(m_model->fileObjectName(mi));
	qint64 size = m_model->fileObjectSize(mi);
	ui->sizeLabel->setText(QString::number(size) + " Byte");
	ui->dimensionLabel->setText("");

	updatePreview(mi);
}

void MainWindow::onListViewClicked(const QModelIndex &index)
{
	// Update preview only when index changed!
	QModelIndex mi = m_proxyModel->mapToSource(index);
	if (ui->objectFileLabel->text() != m_model->fileObjectName(mi)) {
		updatePreview(mi);
	}
}

void MainWindow::onPreviewToggled(bool checked)
{
	if (checked) {
		QModelIndex mi = m_proxyModel->mapToSource(ui->listView->currentIndex());
		updatePreview(mi);
	} else {
		ui->previewLabel->setVisible(checked);
		ui->textBrowser->setVisible(checked);
	}
}

// Show <source> info
void MainWindow::onSourceInfo()
{
	SourceInfoDialog *dlg = new SourceInfoDialog(m_model);
	dlg->show();
}

// Show <fileobject> info
void MainWindow::onObjectInfo()
{
	QModelIndex index = m_proxyModel->mapToSource(ui->listView->currentIndex());
	FileObjectInfoDialog *dlg = new FileObjectInfoDialog(m_model, index);
	dlg->show();
}

void MainWindow::onSaveSectorMap()
{
	m_model->analyze();
}

// Save object as temporary file and launch a viewer application
void MainWindow::onObjectView()
{
	QTemporaryFile *tmpFile = new QTemporaryFile();
	if (!tmpFile->open()) {
		QMessageBox::warning(this, tr("DfXml"), tr("Cannot create temp file"));
		delete tmpFile;
		return;
	}

	QString err;
	QModelIndex mi = m_proxyModel->mapToSource(ui->listView->currentIndex());
	if (!m_model->fileObjectSave(mi, tmpFile->fileName(), err)) {
		QMessageBox::warning(this, tr("DfXml"), err);
		delete tmpFile;
		return;
	} else {
		m_tmpFiles.append(tmpFile);
		statusBar()->showMessage(tr("%1 [TMP SAVED]").arg(tmpFile->fileName()), 2000);
	}

	// USE DESKTOP SERVICES TO OPEN FILES (xdg-open)!
	// The temporary files are deleted when the application is closed!
	if (!QDesktopServices::openUrl(QUrl(tmpFile->fileName(), QUrl::TolerantMode))) {
		qDebug() << __PRETTY_FUNCTION__ << "Cannot open Url";
	}
}

void MainWindow::onSplitterMoved(int pos, int index)
{
	Q_UNUSED(pos)
	Q_UNUSED(index)

	if (ui->previewLabel->isVisible()) {
		QModelIndex mi = m_proxyModel->mapToSource(ui->listView->currentIndex());
		updatePreview(mi);
	}
}

void MainWindow::onObjectViewFilter()
{
	bool ok;
	QString filter = QInputDialog::getText(this, tr("Set Filter"), tr("Filter RegEx"), QLineEdit::Normal, QString(), &ok);
	if (ok) {
		m_proxyModel->setFilterRegExp(QRegExp(filter, Qt::CaseInsensitive, QRegExp::FixedString));
		m_proxyModel->setFilterKeyColumn(0);
	}
}
