#include <QFile>
#include <QDebug>

#include "parser.h"
#include "model.h"

#include <math.h>

DfxmlListModel::DfxmlListModel()
	: QAbstractListModel(),
	  m_parser(0)
{
}

QString DfxmlListModel::sourceFileName() const
{
	return m_source.imageName();
}

DfxmlSource *DfxmlListModel::source()
{
	return &m_source;
}

bool DfxmlListModel::setXmlFile(const QString &xmlFile)
{
	if (m_parser) {
		reset();
		delete m_parser;
	}

	m_parser = new DfxmlParser(xmlFile);
	if (!m_parser->parse()) {
		delete m_parser;
		return false;
	}

	// copy <source>
	m_source = m_parser->source();

	for (int i = 0; i < m_parser->fileObjects().count(); i++) {
		m_items.append(DfxmlModelItem(m_parser->fileObject(i)));
	}

	delete m_parser;
	return true;
}

Qt::ItemFlags DfxmlListModel::flags(const QModelIndex &index) const
{
	if (index.row() < 0 || index.row() >= rowCount() || !index.isValid()) {
		return Qt::NoItemFlags;
	}
	return Qt::ItemIsUserCheckable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

int DfxmlListModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return m_items.size();
}

QVariant DfxmlListModel::data(const QModelIndex &index, int role) const
{
	if (index.row() < 0 || index.row() >= rowCount() || !index.isValid()) {
		return QVariant();
	}

	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		return m_items.at(index.row()).m_object.fileName();
	}

	if (role == Qt::CheckStateRole) {
		return m_items[index.row()].m_checked;
	}

	return QVariant();
}

bool DfxmlListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (index.row() < 0 || index.row() >= rowCount() || !index.isValid()) {
		return false;
	}

	if (role == Qt::CheckStateRole) {
		m_items[index.row()].m_checked = static_cast<Qt::CheckState>(value.toUInt());
	}

	emit dataChanged(index, index);
	return true;
}

QString DfxmlListModel::fileObjectName(const QModelIndex &index)
{
	if (index.row() < 0 || index.row() >= rowCount() || !index.isValid()) {
		return 0;
	}

	return m_items.at(index.row()).m_object.fileName();
}

quint64 DfxmlListModel::fileObjectSize(const QModelIndex &index)
{
	if (index.row() < 0 || index.row() >= rowCount() || !index.isValid()) {
		return 0;
	}

	return m_items.at(index.row()).m_object.fileSize();
}

QByteArray DfxmlListModel::fileObjectData(const QModelIndex &index)
{
	QByteArray ba;
	if (index.row() < 0 || index.row() >= rowCount() || !index.isValid()) {
		return ba;
	}

	// XXX
	QFile inFile(m_source.imageName());
	if (!inFile.exists()) {
		qDebug() << m_source.imageName() << "Not Found";
		return ba;
	}

	inFile.open(QIODevice::ReadOnly);

	// TODO check errors!
	//
	QList<DfxmlByteRun> byteRuns = m_items.at(index.row()).m_object.byteRunList();
	for (int i = 0; i < byteRuns.count(); i++) {
		DfxmlByteRun br = byteRuns.at(i);
		//qDebug() << "READ off:" << br.offset << "img_off:" << br.imgOffset << "len:" << br.len;
		if (inFile.seek(br.imageOffset())) {
			ba.append(inFile.read(br.size()));
		}
	}
	inFile.close();

	return ba;
}

bool DfxmlListModel::fileObjectSave(const QModelIndex &index,
				    const QString &filePath,
				    QString &errorString) const
{
	if (index.row() < 0 || index.row() >= rowCount() || !index.isValid()) {
		errorString = "Invalid ModelIndex";
		//qDebug() << __PRETTY_FUNCTION__ << errorString;
		return false;
	}

	// Open image file
	QFile inFile(m_source.imageName());
	if (!inFile.exists()) {
		errorString = m_source.imageName() + " Not Found";
		//qDebug() << __PRETTY_FUNCTION__ << errorString;
		return false;
	}

	if (!inFile.open(QIODevice::ReadOnly)) {
		errorString = QString("Cannot Open:\n%1\n%2")
					.arg(m_source.imageName())
					.arg(inFile.errorString());

		//qDebug() << __PRETTY_FUNCTION__ << errorString;
		return false;
	}

	QFile outFile(filePath);
	if (!outFile.open(QIODevice::WriteOnly)) {
		errorString = QString("Cannot Open:\n%1\n%2")
					.arg(filePath)
					.arg(outFile.errorString());
		//qDebug() << __PRETTY_FUNCTION__ << errorString;

		inFile.close();
		return false;
	}

	// Copy operation
	bool rc = true;

	QList<DfxmlByteRun> byteRuns = m_items.at(index.row()).m_object.byteRunList();
	for (int i = 0; i < byteRuns.count(); i++) {
		DfxmlByteRun br = byteRuns.at(i);
		qint64 bytesToCopy = br.size();

		//qDebug() << "READ off:" << br.offset
		//	   << "img_off:" << br.imgOffset
		//	   << "len:" << br.len;

		if (inFile.seek(br.imageOffset())) {
			QByteArray ba;
			ba.resize(bytesToCopy + 1);
			qint64 rd = inFile.read(ba.data(), bytesToCopy);
			if (rd < 0) {
				errorString = QString("Read Error: %1")
					.arg(inFile.errorString());
				rc = false;
				break;
			}

			qint64 wr = outFile.write(ba, rd);
			if (wr != rd) {
				errorString = QString("Size Mismatch on copy\n Read:%1 bytes\n Write:%2 bytes")
						.arg(rd)
						.arg(wr)
						.arg(outFile.errorString());
				rc = false;
				break;
			}
		} else {
			rc = false;
			errorString = QString("Seek Error.");
			break;
		}
	}

	if (!rc) {
		qDebug() << __PRETTY_FUNCTION__ << errorString;
	}

	inFile.close();
	outFile.close();

	return rc;
}

DfxmlFileObject DfxmlListModel::fileObject(const QModelIndex &index)
{
	if (index.row() < 0 || index.row() >= rowCount() || !index.isValid()) {
		return DfxmlFileObject();
	}

	return m_items.at(index.row()).m_object;
}

bool DfxmlListModel::fileObjectChecked(const QModelIndex &index)
{
	if (index.row() < 0 || index.row() >= rowCount() || !index.isValid()) {
		return false;
	}

	if (m_items.at(index.row()).m_checked == Qt::Checked) {
		return true;
	}

	return false;
}

bool DfxmlListModel::saveXmlReport(const QString &fileName)
{
	QFile output(fileName);
	if (!output.open(QIODevice::WriteOnly | QIODevice::Text)) {
		return false;
	}

	QXmlStreamWriter stream(&output);
	stream.setAutoFormatting(true);

	stream.writeStartDocument();
	stream.writeStartElement("dfxml");
	stream.writeAttribute("xmloutputversion", "1.0");

	stream.writeStartElement("metadata");
	stream.writeAttribute("xmlns", "http://www.forensicswiki.org/wiki/Category:Digital_Forensics_XML");
	stream.writeAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	stream.writeAttribute("xmlns:dc", "http://purl.org/dc/elements/1.1/");
	stream.writeTextElement("dc:type", "Report");
	stream.writeEndElement();	// </metadata>

	// <source>
	m_source.save(stream);

	// <fileobject> (save obnly checked objecs)
	for (int i = 0; i < m_items.size(); i++) {
		if (m_items[i].m_checked == Qt::Checked) {
			m_items[i].m_object.save(stream);
		}
	}

	stream.writeEndElement();	// </dfxml>
	stream.writeEndDocument();
	output.close();

	return true;
}

void DfxmlListModel::analyze() const
{
	quint64 imgSize    = m_source.imageSize();
	quint64 sectorSize = m_source.sectorSize();

	if (sectorSize == 0) {
		qDebug() << "WARNING: Sector Size = 0 - Cannot save sector map!";
		return;
	}

	quint64 numSectors = imgSize / sectorSize;

	qDebug() << "Sectors:" << numSectors << "of" << sectorSize << "bytes";
	qDebug() << "Image Offset    | Sector         | N.Sectors|Bound|Size|File";
	for (int i = 0; i < m_items.size(); i++) {
			QList<DfxmlByteRun> brl = m_items[i].m_object.byteRunList();
			for (int ss = 0; ss < brl.count(); ss++) {

				quint64 offset       = brl.at(ss).imageOffset();
				quint64 sectorNumber = offset / sectorSize;
				quint64 size         = brl.at(ss).size();
				quint64 sectorSpan   = size / sectorSize;
				quint64 bound        = size % sectorSize; // bytes on remaining

				if (bound) {
					sectorSpan++;
				}

				//("%1 Sector:%2 Span:%3 Bound:%6 Size:%7 File:%8")
				qDebug() << /*QString runString =*/ QString("%1 %2 %3 %6 %7 %8")
							.arg(offset, 16, 16, QChar('0'))
							.arg(sectorNumber, 10, 10, QChar(' '))
							.arg(sectorSpan, 8, 10, QChar(' '))
							.arg(bound, 4, 10, QChar(' '))
							.arg(size, 10, 10, QChar(' '))
							.arg((ss==0)?m_items.at(i).m_object.fileName():"")
							;
				/*qDebug() << runString;*/
			}
	}
}

