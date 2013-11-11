#include <QDebug>

#include "dfxml.h"
#include "byterun.h"
#include "fileobject.h"

DfxmlFileObject::DfxmlFileObject()
{
}

void DfxmlFileObject::loadFromXmlStream(QXmlStreamReader &stream)
{
	// Let's check that we're really getting a <fileobject> tag.
	if (stream.tokenType() != QXmlStreamReader::StartElement
	    && stream.name() == "fileobject") {
		return;
	}

	stream.readNext();

	// Loop over <fileobject> entries until we hit the </fileobject> EndElement.
	while (!(stream.tokenType() == QXmlStreamReader::EndElement &&
		stream.name() == "fileobject")) {
		if (stream.tokenType() == QXmlStreamReader::StartElement) {
			if (stream.name() == "filename") {
				stream.readNext();
				//qDebug() << stream.text().toString();
				if (stream.tokenType() == QXmlStreamReader::Characters) {
					m_fileName = stream.text().toString();
				}
			}
			if (stream.name() == "filesize") {
				stream.readNext();
				//qDebug() << stream.text().toString();
				if (stream.tokenType() == QXmlStreamReader::Characters) {
					setFileSize(stream.text().toString());
				}
			}
			if (stream.name() == "byte_runs") {
				m_byteRuns = Dfxml::readByteRuns(stream);
			}

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
}

void DfxmlFileObject::setByteRuns(const QList<DfxmlByteRun> &byteRuns)
{
	m_byteRuns = byteRuns;
}

void DfxmlFileObject::setFileName(const QString &name)
{
	m_fileName = name;
}

void DfxmlFileObject::setFileSize(const QString &size)
{
	bool ok;
	m_fileSize = size.toULongLong(&ok, 10);
	if (!ok) {
		m_fileSize = 0;
	}
}

void DfxmlFileObject::addByteRun(const DfxmlByteRun &byteRun)
{
	m_byteRuns.append(byteRun);
}

QString DfxmlFileObject::fileName() const
{
	return m_fileName;
}

quint64 DfxmlFileObject::fileSize() const
{
	return m_fileSize;
}

QList<DfxmlByteRun> DfxmlFileObject::byteRunList() const
{
	return m_byteRuns;
}

QString DfxmlFileObject::hashDigest(const QString &type) const
{
	if (type == "MD5" || type == "md5") {
		return m_md5HashDigest;
	}

	if (type == "SHA1" || type == "sha1") {
		return m_sha1HashDigest;
	}

	return QString();
}

void DfxmlFileObject::setHashDigest(const QString &type, const QString &hashdigest)
{
	if (type == "MD5") {
		m_md5HashDigest = hashdigest;
	} else if (type == "SHA1") {
		m_sha1HashDigest = hashdigest;
	} else {
		qDebug() << __PRETTY_FUNCTION__ << "Unknown Hash:"
			 << type << ":" << hashdigest;
	}
}

void DfxmlFileObject::save(QXmlStreamWriter &stream)
{
	stream.writeStartElement("fileobject");
	stream.writeTextElement("filename", m_fileName);
	stream.writeTextElement("filesize", QString::number(m_fileSize));
	Dfxml::writeByteRuns(m_byteRuns, stream);
	stream.writeEndElement();
}

#ifndef QT_NO_DEBUG_OUTPUT
void DfxmlFileObject::debugDump() const
{
	qDebug() << "----------" << __PRETTY_FUNCTION__ << "----------";
	qDebug() << "filename:"  << m_fileName;
	qDebug() << "filesize:"  << m_fileSize;

	for (int i = 0; i < m_byteRuns.count(); i++) {
		m_byteRuns.at(i).debugDump();
	}

	qDebug() << "-------------------------------------------------";
}
#endif


