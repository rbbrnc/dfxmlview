#include "sourceinfodialog.h"
#include "ui_sourceinfodialog.h"

#include "model.h"

#include <QDebug>

SourceInfoDialog::SourceInfoDialog(DfxmlListModel *model, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SourceInfoDialog)
{
	ui->setupUi(this);

	m_source = model->source();

	ui->imageNameLabel->setText(m_source->imageName());
	ui->imageSizeLabel->setText(QString::number(m_source->imageSize()) + " bytes");
	ui->sectorSizeLabel->setText(QString::number(m_source->sectorSize()) + " bytes");
	ui->deviceSectorsLabel->setText(QString::number(m_source->deviceSectors()));
	ui->deviceModelLineEdit->setText(m_source->deviceModel());
	ui->deviceSnLineEdit->setText(m_source->deviceSerialNumber());

	ui->md5HashLabel->setText(m_source->hashDigest("MD5"));
	ui->sha1HashLabel->setText(m_source->hashDigest("SHA1"));

	ui->deviceSnLineEdit->setText(m_source->deviceSerialNumber());
	ui->deviceSnLineEdit->setText(m_source->deviceSerialNumber());

	//2013-03-26T16:12:46+0100
	ui->acquisitionDateTimeEdit->setDisplayFormat("dd MMMM yyyy hh:mm:ss");
	ui->acquisitionDateTimeEdit->setDateTime(m_source->acquisitionDateTime());
	m_acquisitionDateTimeChanged = false;
	connect(ui->acquisitionDateTimeEdit, SIGNAL(editingFinished()),
		this, SLOT(updateAcquisitionDateTime()));

	ui->notePlainTextEdit->setPlainText(m_source->textNote());
}

SourceInfoDialog::~SourceInfoDialog()
{
	delete ui;
}

void SourceInfoDialog::updateAcquisitionDateTime()
{
	m_acquisitionDateTimeChanged = true;
}

void SourceInfoDialog::on_buttonBox_accepted()
{
	if (ui->deviceModelLineEdit->isModified()) {
		m_source->setDeviceModel(ui->deviceModelLineEdit->text());
	}

	if (ui->deviceSnLineEdit->isModified()) {
		m_source->setDeviceSerialNumber(ui->deviceSnLineEdit->text());
	}

	if (m_acquisitionDateTimeChanged) {
		m_source->setAcquisitionDateTime(ui->acquisitionDateTimeEdit->dateTime());
		m_acquisitionDateTimeChanged = false;
	}

	if (ui->notePlainTextEdit->document()->isModified()) {
		m_source->setTextNote(ui->notePlainTextEdit->toPlainText());
	}
}

void SourceInfoDialog::on_buttonBox_rejected()
{
}
