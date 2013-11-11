#include <QFileInfo>
#include <QDebug>

#include "dfxml.h"
#include "source.h"

DfxmlSource::DfxmlSource()
{
	clearData();
}

void DfxmlSource::clearData()
{
	m_imageName.clear();
	m_imageSize = 0;
	m_sectorSize = 0;
	m_deviceSectors = 0;

	m_deviceModel.clear();
	m_deviceSerialNumber.clear();

	m_acquisitionDateTime = QDateTime();

	m_textNote.clear();

	m_volume = DfxmlVolume();
}

void DfxmlSource::loadFromXmlStream(QXmlStreamReader &stream)
{
	// Let's check that we're really getting a <source> tag.
	if (stream.tokenType() != QXmlStreamReader::StartElement
	    && stream.name() == "source") {
		return;
	}

	clearData();
	stream.readNext();

	// Loop over <source> entries until we hit the </source> EndElement.
	while (!(stream.tokenType() == QXmlStreamReader::EndElement &&
		stream.name() == "source")) {
		if (stream.tokenType() == QXmlStreamReader::StartElement) {
			if (stream.name() == "image_filename") {
				stream.readNext();
				if (stream.tokenType() == QXmlStreamReader::Characters) {
					m_imageName = stream.text().toString();
				}
			}
			if (stream.name() == "device_model") {
				stream.readNext();
				if (stream.tokenType() == QXmlStreamReader::Characters) {
					m_deviceModel = stream.text().toString();
				}
			}
			if (stream.name() == "device_sn") {
				stream.readNext();
				if (stream.tokenType() == QXmlStreamReader::Characters) {
					m_deviceSerialNumber = stream.text().toString();
				}
			}
			if (stream.name() == "acquisition_date") {
				stream.readNext();
				if (stream.tokenType() == QXmlStreamReader::Characters) {
					m_acquisitionDateTime = QDateTime::fromString(stream.text().toString());
				}
			}
			if (stream.name() == "sectorsize") {
				stream.readNext();
				if (stream.tokenType() == QXmlStreamReader::Characters) {
					setSectorSize(stream.text().toString());
				}
			}
			if (stream.name() == "devicesectors") {
				stream.readNext();
				if (stream.tokenType() == QXmlStreamReader::Characters) {
					setDeviceSectors(stream.text().toString());
				}
			}
			if (stream.name() == "image_size") {
				stream.readNext();
				if (stream.tokenType() == QXmlStreamReader::Characters) {
					setImageSize(stream.text().toString());
				}
			}
			if (stream.name() == "volume") {
				m_volume.loadFromXmlStream(stream);
			}
			if (stream.name() == "note") {
				stream.readNext();
				if (stream.tokenType() == QXmlStreamReader::Characters) {
					m_textNote = stream.text().toString();
				}
			}
//			<hashdigest type='MD5'>03cfa535ecd7a56dbcc1982f33c20e2e</hashdigest>
			if (stream.name() == "hashdigest") {
				QXmlStreamAttributes attrs = stream.attributes();
				QString type = "unknown";
				if (attrs.hasAttribute("type")) {
				        type = attrs.value("type").toString();
				}
				stream.readNext();
				if (stream.tokenType() == QXmlStreamReader::Characters) {
					setHashDigest(type, stream.text().toString());
				}
			}
	        }

		stream.readNext();
	}

	if (m_deviceSectors == 0) {
		if (m_sectorSize > 0) {
			m_deviceSectors = m_imageSize / m_sectorSize;
		}
	}
	qDebug() << "ImgSize:" << m_imageSize << "SectSize:" << m_sectorSize << "SECTORS:" << m_deviceSectors;
}

void DfxmlSource::setImageFileName(const QString &name)
{
	m_imageName = name;
}

void DfxmlSource::setImageSize(const QString &size)
{
	bool ok;
	m_imageSize = size.toULongLong(&ok, 10);
	if (!ok) {
		m_imageSize = 0;
	}
}

void DfxmlSource::setSectorSize(const QString &size)
{
	bool ok;
	m_sectorSize = size.toULongLong(&ok, 10);
	if (!ok) {
		m_sectorSize = 0;
	}
}

quint64 DfxmlSource::deviceSectors() const
{
	return m_deviceSectors;
}

void DfxmlSource::setDeviceSectors(const QString &size)
{
	bool ok;
	m_deviceSectors = size.toULongLong(&ok, 10);
	if (!ok) {
		m_deviceSectors = 0;
	}
}

QString DfxmlSource::imageName() const
{
	return m_imageName;
}

quint64 DfxmlSource::imageSize() const
{
	return m_imageSize;
}

quint64 DfxmlSource::sectorSize() const
{
	return m_sectorSize;
}

void DfxmlSource::setDeviceModel(const QString &model)
{
	m_deviceModel = model;
}

QString DfxmlSource::deviceModel() const
{
	return m_deviceModel;
}

void DfxmlSource::setDeviceSerialNumber(const QString &sn)
{
	m_deviceSerialNumber = sn;
}

QString DfxmlSource::deviceSerialNumber() const
{
	return m_deviceSerialNumber;
}

void DfxmlSource::setAcquisitionDateTime(const QDateTime &dt)
{
	m_acquisitionDateTime = dt;
}

QDateTime DfxmlSource::acquisitionDateTime() const
{
	return m_acquisitionDateTime;
}

void DfxmlSource::setTextNote(const QString &note)
{
	m_textNote = note;
}

QString DfxmlSource::textNote() const
{
	return m_textNote;
}

QString DfxmlSource::hashDigest(const QString &type) const
{
	if (type == "MD5") {
		return m_md5HashDigest;
	}

	if (type == "SHA1") {
		return m_sha1HashDigest;
	}

	return QString();
}

void DfxmlSource::setHashDigest(const QString &type, const QString &hashdigest)
{
	if (type == "MD5" || type == "md5") {
		m_md5HashDigest = hashdigest;
	} else if (type == "SHA1" || type == "sha1") {
		m_sha1HashDigest = hashdigest;
	} else {
		qDebug() << __PRETTY_FUNCTION__ << "Unknown Hash:"
			 << type << ":" << hashdigest;
	}
}

#ifndef QT_NO_DEBUG_OUTPUT
void DfxmlSource::debugDump() const
{
	qDebug() << "----------" << __PRETTY_FUNCTION__ << "----------";
	qDebug() << "image_name:"  << m_imageName;
	qDebug() << "image_size:"  << m_imageSize;
	qDebug() << "sector_size:" << m_sectorSize;
	qDebug() << "device_model:" << m_deviceModel;
	qDebug() << "device_sn:" << m_deviceSerialNumber;

	m_volume.debugDump();
	qDebug() << "-------------------------------------------------";
}
#endif

void DfxmlSource::save(QXmlStreamWriter &stream)
{
	stream.writeStartElement("source");

	// Save the name without path
	QFileInfo fi(m_imageName);
	stream.writeTextElement("image_filename", fi.fileName());

	stream.writeTextElement("sectorsize", QString::number(m_sectorSize));
	stream.writeTextElement("image_size", QString::number(m_imageSize));

	if (m_deviceSectors > 0) {
		stream.writeTextElement("devicesectors", QString::number(m_deviceSectors));
	}

	if (!m_deviceModel.isEmpty()) {
		stream.writeTextElement("device_model", m_deviceModel);
	}

	if (!m_deviceSerialNumber.isEmpty()) {
		stream.writeTextElement("device_sn", m_deviceSerialNumber);
	}

	if (m_acquisitionDateTime.isValid()) {
		stream.writeTextElement("acquisition_date", m_acquisitionDateTime.toString(Qt::ISODate));
	}

	if (!m_textNote.isEmpty()) {
		stream.writeTextElement("note", m_textNote);
	}

	if (!m_md5HashDigest.isEmpty()) {
		stream.writeTextElement("hashdigest", m_md5HashDigest);
	}
	if (!m_sha1HashDigest.isEmpty()) {
		stream.writeTextElement("hashdigest", m_sha1HashDigest);
	}

	m_volume.save(stream);

	stream.writeEndElement();
}
