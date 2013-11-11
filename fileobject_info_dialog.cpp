#include "fileobject_info_dialog.h"
#include "ui_fileobject_info_dialog.h"

#include "model.h"

#include <QTableWidget>
#include <QDebug>

FileObjectInfoDialog::FileObjectInfoDialog(DfxmlListModel *model, const QModelIndex &index, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::FileObjectInfoDialog)
{
	ui->setupUi(this);

	DfxmlFileObject obj = model->fileObject(index);

	ui->nameLineEdit->setText(obj.fileName());
	ui->sizeLabel->setText(QString::number(obj.fileSize()) + " Bytes");
	ui->md5HashLabel->setText(obj.hashDigest("MD5"));
	ui->sha1HashLabel->setText(obj.hashDigest("SHA1"));

	ui->tableWidget->setSortingEnabled(false);

	QStringList hdr;
	hdr << "File Offset" << "Size" << "Sector" << "Image Offset";
	ui->tableWidget->setHorizontalHeaderLabels(hdr);

	quint64 sectorSize = model->source()->sectorSize();

	QList<DfxmlByteRun> br = obj.byteRunList();
	for (int i = 0; i < br.count(); i++) {
		quint64 imgOffset  = br.at(i).imageOffset();
		qint64 sectorNum;
		if (sectorSize > 0) {
			sectorNum = imgOffset / sectorSize;
		} else {
			sectorNum = -1;
		}
/*
		QString run = QString("File Offset: %1 Size: %2 bytes - Sector: %3 at Offset: %4")
				.arg(br.at(i).offset())
				.arg(br.at(i).size())
				.arg(sectorNum)
				.arg(imgOffset);
		qDebug() << __PRETTY_FUNCTION__ << run;
*/

		ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
		QTableWidgetItem *sectorItem = new QTableWidgetItem(QString::number(sectorNum));
		QTableWidgetItem *sizeItem = new QTableWidgetItem(QString::number(br.at(i).size()));
		QTableWidgetItem *ioffItem = new QTableWidgetItem(QString::number(imgOffset));
		QTableWidgetItem *foffItem = new QTableWidgetItem(QString::number(br.at(i).offset()));

		ui->tableWidget->setItem(i, 0, foffItem);
		ui->tableWidget->setItem(i, 1, sizeItem);
		ui->tableWidget->setItem(i, 2, sectorItem);
		ui->tableWidget->setItem(i, 3, ioffItem);
	}
	ui->tableWidget->setSortingEnabled(true);
}

FileObjectInfoDialog::~FileObjectInfoDialog()
{
	delete ui;
}

void FileObjectInfoDialog::on_buttonBox_accepted()
{
}

void FileObjectInfoDialog::on_buttonBox_rejected()
{
}
